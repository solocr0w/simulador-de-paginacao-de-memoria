#include "processo.h"
#include <stdio.h>
#include <stdlib.h>
#include "tabela-pagina.h"
#include "pagina.h"
#include "memoria.h"

Processo* processo_criar(int pid, int tamanho, int tamanho_pagina){
    Processo *processo = malloc(sizeof(Processo));
    if (!processo) {
        fprintf(stderr, "Erro ao alocar memória para o processo.\n");
        return NULL;
    }
    
    processo->pid = pid;
    processo->tamanho = tamanho;
    processo->num_paginas = (tamanho + tamanho_pagina - 1) / tamanho_pagina; // Arredonda para cima
    processo->tabela = tabela_paginas_criar(pid, processo->num_paginas);
    
    if (!processo->tabela) {
        free(processo);
        fprintf(stderr, "Erro ao criar a tabela de páginas do processo.\n");
        return NULL;
    }
    
    return processo;
}