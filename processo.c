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

//Aloca o processo dentro da memória física
void processo_alocar_memoria(Simulador *sim, int pid, int tamanho, int tamanho_pagina) {

    Processo *processo = sim->processos[pid];

    if (!processo || !processo->tabela) {
        fprintf(stderr, "Processo ou tabela de páginas inválidos.\n");
        return;
    }

    // Aloca as páginas na memória física
    for (int i = 0; i < processo->num_paginas; i++) {
        Pagina *pagina = &processo->tabela->paginas[i];
        pagina->frame = memoria_alocar_frame_livre(sim, pid, i);
        pagina->presente = (pagina->frame != -1);
        pagina->modificada = false;
        pagina->referenciada = false;
        pagina->tempo_carga = pagina->presente? sim->tempo_sistema :0; // Define o tempo de carga com o tempo do sistema ou 0 se não estiver presente
        pagina->ultimo_acesso = pagina->presente? sim->tempo_sistema :0; // Define o último acesso com o tempo do sistema ou 0 se não estiver presente
    }
    
}