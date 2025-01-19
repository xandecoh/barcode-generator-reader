#ifndef COMPARTILHADO_H
#define COMPARTILHADO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estrutura para armazenar o código de barras
typedef struct {
    char* codigo;          // Código de barras de 8 dígitos
    int largura_codigo;    // Largura do código de barras
    int altura_codigo;     // Altura do código de barras
    int margem;            // Margem
    int pixels_area;       // Tamanho de cada segmento do código
} CodigoDeBarras;

// Declaração dos padrões de bits para L-code e R-code
extern const char *codigo_esq[10];
extern const char *codigo_dir[10];

// Declaração da função de validação do código
int validar_codigo(const char* codigo);

#endif
