#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> // Para a função access
#include "compartilhado.h" // Inclui o cabeçalho com as funções compartilhadas

// Função para criar o código de barras no formato PBM
void criar_codigo_barras(CodigoDeBarras* cdb, FILE* arquivo) {
    int largura_total = cdb->largura_codigo * cdb->pixels_area + 2 * cdb->margem;
    int altura_total = cdb->altura_codigo + 2 * cdb->margem;

    fprintf(arquivo, "P1\n%d %d\n", largura_total, altura_total);

    for (int y = 0; y < altura_total; y++) {
        for (int x = 0; x < largura_total; x++) {
            int dentro_margem_vertical = (y >= cdb->margem && y < altura_total - cdb->margem);
            int dentro_margem_horizontal = (x >= cdb->margem && x < largura_total - cdb->margem);

            if (!dentro_margem_vertical || !dentro_margem_horizontal) {
                fprintf(arquivo, "0 ");
                continue;
            }

            int posicao = (x - cdb->margem) / cdb->pixels_area;
            char bit = '0';

            if (posicao < 3) { // Início "101"
                bit = "101"[posicao];
            } else if (posicao < 31) { // Lado esquerdo
                bit = codigo_esq[cdb->codigo[(posicao - 3) / 7] - '0'][(posicao - 3) % 7];
            } else if (posicao < 36) { // Separador "01010"
                bit = "01010"[posicao - 31];
            } else if (posicao < 64) { // Lado direito
                bit = codigo_dir[cdb->codigo[(posicao - 36) / 7 + 4] - '0'][(posicao - 36) % 7];
            } else { // Final "101"
                bit = "101"[posicao - 64];
            }

            fprintf(arquivo, "%c ", bit);
        }
        fprintf(arquivo, "\n");
    }
}

// Função principal
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <identificador> [margem] [pixels_por_area] [altura] [arquivo_saida]\n", argv[0]);
        return 1;
    }

    CodigoDeBarras cdb;
    cdb.codigo = argv[1];
    if (!validar_codigo(cdb.codigo)) {
        printf("Erro: Identificador inválido. Certifique-se de que ele tenha 8 dígitos e um dígito verificador correto.\n");
        return 1;
    }

    cdb.margem = (argc > 2) ? atoi(argv[2]) : 4;
    cdb.pixels_area = (argc > 3) ? atoi(argv[3]) : 3;
    cdb.altura_codigo = (argc > 4) ? atoi(argv[4]) : 50;
    cdb.largura_codigo = 67;

    const char* nome_arquivo = (argc > 5) ? argv[5] : "codigo_barras.pbm";

    if (access(nome_arquivo, F_OK) == 0) {
        char resposta;
        printf("Aviso: O arquivo %s já existe e será sobrescrito se confirmado. Deseja continuar? (s/n): ", nome_arquivo);
        scanf(" %c", &resposta);
        if (resposta != 's' && resposta != 'S') {
            printf("Operação cancelada. O arquivo existente não foi sobrescrito.\n");
            return 1;
        }
    }

    FILE* arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        printf("Erro ao criar o arquivo %s.\n", nome_arquivo);
        return 1;
    }

    criar_codigo_barras(&cdb, arquivo);
    fclose(arquivo);

    printf("Código de barras salvo em %s.\n", nome_arquivo);
    return 0;
}
