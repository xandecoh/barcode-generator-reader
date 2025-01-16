#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int **leitorim(const char *arquivim, int *largura, int *altura) {
    FILE *arquivoleu = fopen(arquivim, "r");
    if (!arquivoleu) {
        printf("Erro: O arquivo %s não foi encontrado.\n", arquivim);
        return 0;
    }

    char typearc[3];
    fscanf(arquivoleu, "%2s", typearc);
    if (strcmp(typearc, "P1") != 0) {
        printf("Formato de imagem inválido. O tipo deve ser P1.\n");
        fclose(arquivoleu);
        return 0;
    }

    char c;
    while ((c = fgetc(arquivoleu)) == '#') {
        while (fgetc(arquivoleu) != '\n');
    }
    ungetc(c, arquivoleu);

    fscanf(arquivoleu, "%d %d", largura, altura);
    if (*largura <= 0 || *altura <= 0) {
        printf("Erro: Dimensões inválidas na imagem.\n");
        fclose(arquivoleu);
        return 0;
    }

    int **matrizi = malloc(*altura * sizeof(int *));
    if (!matrizi) {
        printf("Erro: Falha na alocação de memória para a matriz de imagem.\n");
        fclose(arquivoleu);
        return 0;
    }

    for (int i = 0; i < *altura; i++) {
        matrizi[i] = malloc(*largura * sizeof(int));
        if (!matrizi[i]) {
            printf("Erro: Falha na alocação de memória para a linha %d da matriz.\n", i);
            for (int k = 0; k < i; k++) {
                free(matrizi[k]);
            }
            free(matrizi);
            fclose(arquivoleu);
            return (int **)0;
        }

        for (int j = 0; j < *largura; j++) {
            fscanf(arquivoleu, "%d", &matrizi[i][j]);
        }
    }

    fclose(arquivoleu);
    return matrizi;
}

void calcular_margem(int **matriz, int largura, int altura, int *margem_sup) {
    *margem_sup = 0;

    for (int linha = 0; linha < altura; linha++) {
        int soma_pixels = 0;
        for (int coluna = 0; coluna < largura; coluna++) {
            soma_pixels += matriz[linha][coluna];
        }

        if (soma_pixels > 0) {
            break;
        }

        (*margem_sup)++;
        if (*margem_sup >= altura) {
            printf("Erro: Nenhuma margem superior identificada.\n");
            *margem_sup = altura;
            break;
        }
    }
}

void decodificar_barras(int **matriz, int largura_img, int altura_img) {
    int margem_superior;
    calcular_margem(matriz, largura_img, altura_img, &margem_superior);

    int largura_total_barras = largura_img - 2 * margem_superior;
    int largura_segmento = (largura_total_barras + 66) / 67;
    int linha_media = margem_superior + altura_img / 2;
    char sequencia_bits[68] = {0};

    for (int i = 0; i < 67; i++) {
        int total_brancos = 0;
        int total_pretos = 0;

        for (int p = 0; p < largura_segmento; p++) {
            int valor_pixel = matriz[linha_media][margem_superior + i * largura_segmento + p];
            if (valor_pixel == 0) {
                total_brancos++;
            } else {
                total_pretos++;
            }
        }

        sequencia_bits[i] = (total_pretos > total_brancos) ? '1' : '0';
    }

    if (strncmp(sequencia_bits, "101", 3) != 0 || strncmp(sequencia_bits + 64, "101", 3) != 0 || strncmp(sequencia_bits + 31, "01010", 5) != 0) {
        printf("Erro: Estrutura de código de barras inválida.\n");
        return;
    }

    printf("Código de barras decodificado: ");
    const char *padrao_esquerda[] = {
        "0001101", "0011001", "0010011", "0111101", "0100011",
        "0110001", "0101111", "0111011", "0110111", "0001011"
    };
    const char *padrao_direita[] = {
        "1110010", "1100110", "1101100", "1000010", "1011100",
        "1001110", "1010000", "1000100", "1001000", "1110100"
    };

    for (int i = 0; i < 2; i++) {
        int inicio = (i == 0) ? 3 : 36;
        int fim = (i == 0) ? 31 : 64;
        const char **padrao = (i == 0) ? padrao_esquerda : padrao_direita;

        for (int j = inicio; j < fim; j += 7) {
            int encontrado = 0;
            for (int k = 0; k < 10; k++) {
                if (strncmp(sequencia_bits + j, padrao[k], 7) == 0) {
                    printf("%d", k);
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                printf("?");
            }
        }
    }
    printf("\n");
}

int main() {
    char nome_imagem[100];
    printf("Nome do código de barras (máximo 99 caracteres): ");
    if (scanf(" %99s", nome_imagem) != 1) {
        printf("Erro: Entrada inválida.\n");
        return 1;
    }

    int largura, altura;
    int **imagem = leitorim(nome_imagem, &largura, &altura);
    if (!imagem) {
        return 1;
    }

    decodificar_barras(imagem, largura, altura);

    for (int i = 0; i < altura; i++) {
        free(imagem[i]);
    }
    free(imagem);

    printf("Decodificação finalizada com sucesso.\n");
    return 0;
}

