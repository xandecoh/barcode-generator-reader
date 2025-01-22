#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_SPACING 4
#define DEFAULT_MODULE_WIDTH 3
#define DEFAULT_HEIGHT 50
#define DEFAULT_FILENAME "codigo_de_de_barras.pbm"

// Tabelas de codifica��o L-code e R-code
const char *L_CODE[10] = {"0001101", "0011001", "0010011", "0111101", "0100011", "0110001", "0101111", "0111011", "0110111", "0001011"};
const char *R_CODE[10] = {"1110010", "1100110", "1101100", "1000010", "1011100", "1001110", "1010000", "1000100", "1001000", "1110100"};

// Fun��o para validar o identificador e calcular o d�gito verificador
int calcular_digito_verificador(const char *identificador) {
    int soma = 0;
    for (int i = 0; i < 7; i++) {
        int digito = identificador[i] - '0';
        soma += (i % 2 == 0) ? digito * 3 : digito;
    }
    return (10 - (soma % 10)) % 10;
}

bool validar_identificador(const char *identificador) {
    if (strlen(identificador) != 8) return false;
    for (int i = 0; i < 8; i++) {
        if (identificador[i] < '0' || identificador[i] > '9') return false;
    }
    int digito_verificador = calcular_digito_verificador(identificador);
    return (digito_verificador == identificador[7] - '0');
}

// Fun��o para gerar o c�digo de barras em formato PBM
void gerar_codigo_pbm(const char *identificador, int espacamento, int largura_modulo, int altura, const char *nome_arquivo) {
    // Largura total do c�digo de barras
    int largura_codigo = 3 + (7 * 3) + 4 + (7 * 3) + 3; // Marcadores + d�gitos
    int largura_total = largura_codigo * largura_modulo + 2 * espacamento;
    int altura_total = altura + 2 * espacamento;

    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        perror("Erro ao criar o arquivo");
        exit(EXIT_FAILURE);
    }

    // Cabe�alho PBM
    fprintf(arquivo, "P1\n%d %d\n", largura_total, altura_total);

    // Gerar a matriz do c�digo de barras
    for (int y = 0; y < altura_total; y++) {
        for (int x = 0; x < largura_total; x++) {
            bool preenchido = false;

            // Verificar espa�amento superior e inferior
            if (y < espacamento || y >= altura_total - espacamento) {
                preenchido = false;
            } else if (x < espacamento || x >= largura_total - espacamento) {
                preenchido = false;
            } else {
                int pos_x = (x - espacamento) / largura_modulo;
                int area = pos_x / 7;
                int sub_pos = pos_x % 7;

                if (area == 0 || area == 8) {
                    // Marcadores de in�cio e fim (101)
                    preenchido = (sub_pos == 0 || sub_pos == 2);
                } else if (area == 4) {
                    // Marcador central (01010)
                    preenchido = (sub_pos == 1 || sub_pos == 3);
                } else {
                    // D�gitos (L-code ou R-code)
                    int indice_digito = (area < 4) ? area - 1 : area - 5;
                    int digito = identificador[indice_digito] - '0';
                    const char *codigo = (area < 4) ? L_CODE[digito] : R_CODE[digito];
                    preenchido = (codigo[sub_pos] == '1');
                }
            }

            fprintf(arquivo, "%d ", preenchido ? 1 : 0); // Adicionando espa�o para melhor formata��o
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    printf("Arquivo %s gerado com sucesso!\n", nome_arquivo);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <identificador> [<espacamento> <largura_modulo> <altura> <nome_arquivo>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *identificador = argv[1];
    int espacamento = (argc > 2) ? atoi(argv[2]) : DEFAULT_SPACING;
    int largura_modulo = (argc > 3) ? atoi(argv[3]) : DEFAULT_MODULE_WIDTH;
    int altura = (argc > 4) ? atoi(argv[4]) : DEFAULT_HEIGHT;
    const char *nome_arquivo = (argc > 5) ? argv[5] : DEFAULT_FILENAME;

    if (!validar_identificador(identificador)) {
        fprintf(stderr, "Identificador inv�lido. Certifique-se de que ele possui 8 d�gitos num�ricos e um d�gito verificador v�lido.\n");
        return EXIT_FAILURE;
    }

    gerar_codigo_pbm(identificador, espacamento, largura_modulo, altura, nome_arquivo);
    return EXIT_SUCCESS;
}
