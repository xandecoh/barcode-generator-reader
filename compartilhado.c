#include "compartilhado.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Arrays com os padrões de bits para os dois lados do código
const char *codigo_esq[] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

const char *codigo_dir[] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

// Função que valida um código de barras EAN-8
// Verifica se o código tem 8 dígitos e calcula o dígito verificador usando a fórmula padrão.
int validar_codigo(const char* codigo) {
    if (strlen(codigo) != 8) {
        printf("Erro: O código fornecido deve ser um EAN-8 válido com 8 dígitos numéricos.\n");
        return 0;
    }

    for (int i = 0; i < 8; i++) {
        if (!isdigit(codigo[i])) {
            printf("Erro: O código contém caracteres não numéricos.\n");
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
