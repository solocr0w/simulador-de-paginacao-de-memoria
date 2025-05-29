#include "tabela-pagina.h"
#include <stdio.h>
#include <stdlib.h>
#include "pagina.h"
#include "memoria.h"

// Cria uma nova tabela de páginas para um processo
TabelaPaginas* tabela_paginas_criar(int pid, int num_paginas){
    TabelaPaginas *tabela = malloc(sizeof(TabelaPaginas));
    if (!tabela) {
        fprintf(stderr, "Erro ao alocar memória para a tabela de páginas.\n");
        return NULL;
    }
    
    tabela->paginas = malloc(sizeof(Pagina) * num_paginas);
    if (!tabela->paginas) {
        free(tabela);
        fprintf(stderr, "Erro ao alocar memória para as páginas.\n");
        return NULL;
    }
    
    for (int i = 0; i < num_paginas; i++) {
        tabela->paginas[i].frame = -1; // Sem frame alocado
        tabela->paginas[i].presente = 0; // Inicialmente não está presente
        tabela->paginas[i].modificada = 0; // Inicialmente não modificada
        tabela->paginas[i].ultimo_acesso = 0; // Inicialmente não acessada
        tabela->paginas[i].referenciada = 0; // Inicialmente não referenciada
    }
    
    tabela->tamanho = num_paginas;
    tabela->pid = pid;
    
    return tabela;
}


void tabela_paginas_remover_pagina(Processo *proc, int frame_removido) {
        for (int j = 0; j < proc->num_paginas; j++) {
            Pagina *pagina = &proc->tabela->paginas[j];
            if (pagina->presente && pagina->frame == frame_removido) {
                pagina->presente = false;
                pagina->frame = -1;
                return;
            }
        }
}


//Atualiza a o frame em uma tabela de pagina e marcar como presente
void tabela_paginas_atualizar_presente(TabelaPaginas *tabela, int num_pagina, int frame){
    if (num_pagina < 0 || num_pagina >= tabela->tamanho) {
        fprintf(stderr, "Número de página inválido: %d\n", num_pagina);
        return;
    }
    
    tabela->paginas[num_pagina].presente = 1; // Marca como presente
    tabela->paginas[num_pagina].frame = frame; // Atualiza o frame alocado
}

// Marca uma página como não presente na memória física
void tabela_paginas_atualizar_nao_presente(TabelaPaginas *tabela, int num_pagina){
    
    if (num_pagina < 0 || num_pagina >= tabela->tamanho) {
        fprintf(stderr, "Número de página inválido: %d\n", num_pagina);
        return;
    }
    
    tabela->paginas[num_pagina].presente = 0; // Marca como não presente
    tabela->paginas[num_pagina].frame = -1; // Reseta o frame alocado
}