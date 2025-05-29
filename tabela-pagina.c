#include "tabela-pagina.h"
#include "pagina.h"
#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>

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
        tabela->paginas[i].tempo_carga = 0; // Inicialmente não carregada
        tabela->paginas[i].ultimo_acesso = 0; // Inicialmente não acessada
        tabela->paginas[i].referenciada = 0; // Inicialmente não referenciada
    }
    
    tabela->tamanho = num_paginas;
    tabela->pid = pid;
    
    return tabela;
}