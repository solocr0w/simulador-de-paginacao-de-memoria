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

//Extrai o número da página e o deslocamento de um endereço virtual
void processo_extrair_endereco(Processo *processo, int endereco_virtual, int *num_pagina, int *deslocamento, int tamanho_pagina){

    if (!processo || !num_pagina || !deslocamento) {
        fprintf(stderr, "Processo ou ponteiros inválidos.\n");
        return;
    }
    
    *num_pagina = endereco_virtual / tamanho_pagina; // Número da página
    *deslocamento = endereco_virtual % tamanho_pagina; // Deslocamento dentro da página
    
    if (*num_pagina >= processo->num_paginas) {
        fprintf(stderr, "Endereço virtual fora dos limites do processo.\n");
        *num_pagina = -1; // Indica erro
        *deslocamento = -1; // Indica erro
    }

}



// Busca por um processo pelo PID
Processo* processo_busca(Simulador *sim, int pid){
    if (!sim || !sim->processos) {
        return NULL;
    }
    for (int i = 0; i < sim->num_processos; i++) {
        if (sim->processos[i]->pid == pid) {
            return sim->processos[i];
        }
    }
    return NULL;
}

//Aloca o processo dentro da memória física
void processo_alocar_memoria(Simulador *sim, int pid, int tamanho, int tamanho_pagina) {

    Processo *processo = processo_busca(sim, pid);

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

// Destrói o processo (libera memória)
void processo_destruir(Simulador *sim, Processo *processo){
    if (!processo) {
        return;
    }

    // Libera a tabela de páginas
    if (processo->tabela) {
        free(processo->tabela->paginas);
        free(processo->tabela);
    }

    // Libera o processo
    free(processo);

    // Remove o processo do simulador
    for (int i = 0; i < sim->num_processos; i++) {
        if (sim->processos[i]->pid == processo->pid) {
            for (int j = i; j < sim->num_processos - 1; j++) {
                sim->processos[j] = sim->processos[j + 1];
            }
            sim->num_processos--;
            break;
        }
    }

    //Desaloca a memória física ocupada pelo processo
    for (int i = 0; i < processo->num_paginas; i++) {
        Pagina *pagina = &processo->tabela->paginas[i];
        if (pagina->presente) {
            memoria_liberar_frame(sim, pagina->frame);
            tabela_paginas_atualizar_nao_presente(processo->tabela, i);
        }
    }

}