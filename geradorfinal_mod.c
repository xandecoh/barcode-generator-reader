#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estruturas para armazenar o código de barras
typedef struct {
    char* codigo;          // Código de barras de 8 dígitos
    int largura_codigo;    // Largura do código de barras
    int altura_codigo;     // Altura do código de barras
    int margem;            // Margem
    int pixels_area;       // Tamanho de cada segmento do código
} CodigoDeBarras;

// Arrays com os padrões de bits para os dois lados do código
const char *codigo_esq[] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

const char *codigo_dir[] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

// Função para validar o código de barras inserido
int validar_codigo(const char* codigo) {
    if (strlen(codigo) != 8) {
        printf("Código deve ter 8 dígitos.\n");
        return 0;
    }

    for (int i = 0; i < 8; i++) {
        if (!isdigit(codigo[i])) {
            printf("Código contém caracteres não numéricos.\n");
            return 0;
        }
    }

    int soma = 0;
    for (int i = 0; i < 7; i++) {
        int digito = codigo[i] - '0';
        soma += (i % 2 == 0) ? digito * 3 : digito;
    }

    int digito_verificador = (10 - (soma % 10)) % 10;
    return digito_verificador == codigo[7] - '0';
}

// Função para capturar o código do usuário
void obter_codigo_valido(char** codigo) {
    *codigo = (char*)malloc(9 * sizeof(char));  // Alocação dinâmica para o código
    if (*codigo == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);  // Finaliza o programa em caso de erro na alocação
    }

    do {
        printf("Digite o código de 8 dígitos: ");
        scanf("%8s", *codigo);
        if (!validar_codigo(*codigo)) {
            printf("Código inválido.\n");
        }
    } while (!validar_codigo(*codigo));
}

// Função para obter um valor inteiro de forma segura
int obter_inteiro(const char* mensagem) {
    int valor;
    while (1) {
        printf("%s", mensagem);
        if (scanf("%d", &valor) == 1) break;
        printf("Por favor, insira um número válido.\n");
        while (getchar() != '\n');  // Limpa o buffer de entrada
    }
    return valor;
}

// Função para criar o código de barras no formato PBM e salvar em arquivo
void criar_codigo_barras(CodigoDeBarras* cdb, FILE* arquivo) {
    int largura_total = cdb->largura_codigo * cdb->pixels_area + 2 * cdb->margem;
    int altura_total = cdb->altura_codigo + 2 * cdb->margem;

    fprintf(arquivo, "P1\n%d %d\n", largura_total, altura_total);

    for (int y = 0; y < altura_total; y++) {
        for (int x = 0; x < largura_total; x++) {
            int dentro_margem_vertical = (y >= cdb->margem && y < altura_total - cdb->margem);
            int dentro_margem_horizontal = (x >= cdb->margem && x < largura_total - cdb->margem);

            // Posições com margens
            if (!dentro_margem_vertical || !dentro_margem_horizontal) {
                fprintf(arquivo, "0 ");
                continue;
            }

            // Coordenada ajustada ao início do código de barras
            int posicao = (x - cdb->margem) / cdb->pixels_area;
            char bit = '0';

            if (posicao < 3) {  // Início "101"
                bit = "101"[posicao];
            } else if (posicao < 31) {  // Lado esquerdo
                bit = codigo_esq[cdb->codigo[(posicao - 3) / 7] - '0'][(posicao - 3) % 7];
            } else if (posicao < 36) {  // Separador "01010"
                bit = "01010"[posicao - 31];
            } else if (posicao < 64) {  // Lado direito
                bit = codigo_dir[cdb->codigo[(posicao - 36) / 7 + 4] - '0'][(posicao - 36) % 7];
            } else {  // Final "101"
                bit = "101"[posicao - 64];
            }

            fprintf(arquivo, "%c ", bit);
        }
        fprintf(arquivo, "\n");
    }
}

// Função para liberar a memória alocada para o código
void liberar_codigo(char* codigo) {
    free(codigo);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <identificador> [margem] [pixels_por_area] [altura] [arquivo_saida]\n", argv[0]);
        return 1;
    }

    CodigoDeBarras cdb;
    cdb.codigo = argv[1];
    if (!validar_codigo(cdb.codigo)) {
        printf("Identificador inválido. Certifique-se de que ele tenha 8 dígitos e um dígito verificador correto.\n");
        return 1;
    }

    cdb.margem = (argc > 2) ? atoi(argv[2]) : 4;
    cdb.pixels_area = (argc > 3) ? atoi(argv[3]) : 3;
    cdb.altura_codigo = (argc > 4) ? atoi(argv[4]) : 50;
    cdb.largura_codigo = 67;

    const char* nome_arquivo = (argc > 5) ? argv[5] : "codigo_barras.pbm";

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
