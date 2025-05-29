#ifndef TABELA_PAGINAS_H
#define TABELA_PAGINAS_H

#include "pagina.h"

typedef struct {

    Pagina *paginas;      // Array de páginas
    int tamanho;          // Número de páginas na tabela
    int pid;              // PID do processo dono da tabela
    
} TabelaPaginas;

// Cria uma nova tabela de páginas para um processo
TabelaPaginas* tabela_paginas_criar(int pid, int num_paginas);

// Destrói a tabela (libera memória)
void tabela_paginas_destruir(TabelaPaginas *tabela);

// Busca uma página pelo número (retorna NULL se inválido)
Pagina* tabela_paginas_buscar(TabelaPaginas *tabela, int num_pagina);

// Marca uma página como presente na memória física
void tabela_paginas_atualizar_presente(TabelaPaginas *tabela, int num_pagina, int frame);

// Reseta os bits de referência (útil para algoritmos como Clock)
void tabela_paginas_resetar_referencias(TabelaPaginas *tabela);

#endif