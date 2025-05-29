#ifndef PROCESSO_H
#define PROCESSO_H

#include <stdbool.h>
#include "tabela-pagina.h"

typedef struct {

    int pid;                // Identificador do processo
    int tamanho;            // Tamanho total em bytes
    int num_paginas;        // Número de páginas (tamanho / tamanho_pagina)
    TabelaPaginas *tabela;  // Tabela de páginas do processo
    
} Processo;

// Cria um novo processo, mas não aloca na memoria
Processo* processo_criar(int pid, int tamanho, int tamanho_pagina);

// Traduz um endereço virtual para número de página e deslocamento
void processo_extrair_endereco(Processo *processo, int endereco_virtual, int *num_pagina, int *deslocamento, int tamanho_pagina);

#endif