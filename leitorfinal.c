#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para ler o arquivo PBM e alocar a matriz de pixels
int **carregarImagemPBM(const char *nomeArquivo, int *largura, int *altura) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro: Não foi possível abrir o arquivo %s.\n", nomeArquivo);
        return NULL;
    }

    // Verifica o tipo de arquivo (OBRIGATORIAMENTE deve ser "P1")
    char tipo[3];
    if (fscanf(arquivo, "%2s", tipo) != 1 || strcmp(tipo, "P1") != 0) {
        printf("Erro: Formato inválido. O arquivo deve ser do tipo P1.\n");
        fclose(arquivo);
        return NULL;
    }

    // Ignora comentários e lê as dimensões da imagem
    int c;
    while ((c = fgetc(arquivo)) == '#') {
        while (fgetc(arquivo) != '\n');
    }
    ungetc(c, arquivo);

    if (fscanf(arquivo, "%d %d", largura, altura) != 2 || *largura <= 0 || *altura <= 0) {
        printf("Erro: Dimensões inválidas no arquivo.\n");
        fclose(arquivo);
        return NULL;
    }

    // Aloca dinamicamente a matriz de pixels
    int **imagem = malloc(*altura * sizeof(int *));
    if (!imagem) {
        printf("Erro: Falha na alocação de memória para a matriz.\n");
        fclose(arquivo);
        return NULL;
    }

    for (int i = 0; i < *altura; i++) {
        imagem[i] = malloc(*largura * sizeof(int));
        if (!imagem[i]) {
            printf("Erro: Falha na alocação de memória para a linha %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(imagem[j]);
            }
            free(imagem);
            fclose(arquivo);
            return NULL;
        }
        for (int j = 0; j < *largura; j++) {
            if (fscanf(arquivo, "%d", &imagem[i][j]) != 1) {
                printf("Erro: Dados inconsistentes no arquivo.\n");
                for (int k = 0; k <= i; k++) {
                    free(imagem[k]);
                }
                free(imagem);
                fclose(arquivo);
                return NULL;
            }
        }
    }

    fclose(arquivo);
    return imagem;
}

// Função para calcular a margem superior da imagem
int calcularMargemSuperior(int **imagem, int largura, int altura) {
    for (int i = 0; i < altura; i++) {
        int soma = 0;
        for (int j = 0; j < largura; j++) {
            soma += imagem[i][j];
        }
        if (soma > 0) {
            return i;
        }
    }
    return altura; // Retorna altura caso toda a imagem seja branca
}

// Função para decodificar o código de barras presente na matriz
void decodificarCodigoBarras(int **imagem, int largura, int altura) {
    int margemSuperior = calcularMargemSuperior(imagem, largura, altura);
    if (margemSuperior >= altura) {
        printf("Erro: Não foi possível identificar a margem superior.\n");
        return;
    }

    int larguraUtil = largura - 2 * margemSuperior;
    int larguraSegmento = (larguraUtil + 66) / 67;
    int linhaCentral = margemSuperior + altura / 2;

    char bits[68] = {0};
    for (int i = 0; i < 67; i++) {
        int brancos = 0, pretos = 0;
        for (int j = 0; j < larguraSegmento; j++) {
            int pixel = imagem[linhaCentral][margemSuperior + i * larguraSegmento + j];
            if (pixel == 0) {
                brancos++;
            } else {
                pretos++;
            }
        }
        bits[i] = (pretos > brancos) ? '1' : '0';
    }

    // Valida estrutura do código de barras
    if (strncmp(bits, "101", 3) != 0 || strncmp(bits + 64, "101", 3) != 0 || strncmp(bits + 31, "01010", 5) != 0) {
        printf("Erro: Estrutura do código de barras inválida.\n");
        return;
    }

    printf("Código de barras decodificado: ");
    const char *padraoEsquerda[] = {"0001101", "0011001", "0010011", "0111101", "0100011", "0110001", "0101111", "0111011", "0110111", "0001011"};
    const char *padraoDireita[] = {"1110010", "1100110", "1101100", "1000010", "1011100", "1001110", "1010000", "1000100", "1001000", "1110100"};

    for (int grupo = 0; grupo < 2; grupo++) {
        int inicio = (grupo == 0) ? 3 : 36;
        const char **padrao = (grupo == 0) ? padraoEsquerda : padraoDireita;
        for (int j = inicio; j < inicio + 28; j += 7) {
            int encontrado = 0;
            for (int k = 0; k < 10; k++) {
                if (strncmp(bits + j, padrao[k], 7) == 0) {
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

// Função principal
int main() {
    char nomeArquivo[100];
    printf("Digite o nome do arquivo PBM (máximo 99 caracteres): ");
    if (scanf(" %99s", nomeArquivo) != 1) {
        printf("Erro: Entrada inválida.\n");
        return 1;
    }

    int largura, altura;
    int **imagem = carregarImagemPBM(nomeArquivo, &largura, &altura);
    if (!imagem) {
        return 1;
    }

    decodificarCodigoBarras(imagem, largura, altura);

    for (int i = 0; i < altura; i++) {
        free(imagem[i]);
    }
    free(imagem);

    printf("Processo concluído com sucesso.\n");
    return 0;
}
