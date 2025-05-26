#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"
#include "memoria.h"
#include "processo.h"
#include "tabela-pagina.h"

Simulador* criarSimulador(int tamanho_pagina, int tamanho_memoria_fisica, AlgoritmoSubstituicao algoritmoEscolhido, int numFrames) {
    Simulador *sim = malloc(sizeof(Simulador));
    if (!sim) {
        fprintf(stderr, "Erro ao alocar memória para o simulador.\n");
        return NULL;
    }
    sim->tamanho_pagina = tamanho_pagina;
    sim->tamanho_memoria_fisica = tamanho_memoria_fisica;
    sim->algoritmo = algoritmoEscolhido;
    sim->memoria = memoria_criar(tamanho_memoria_fisica, tamanho_pagina, numFrames);
    if (!sim->memoria) {
        free(sim);
        fprintf(stderr, "Erro ao criar memória física.\n");
        return NULL;
    }
    sim->processos = malloc(sizeof(Processo*) * numFrames);
    if (!sim->processos) {
        memoria_destruir(sim->memoria);
        free(sim);
        fprintf(stderr, "Erro ao alocar memória para os processos.\n");
        return NULL;
    }
    sim->num_processos = 0;
    sim->total_acessos = 0;
    sim->total_page_faults = 0;
    printf("Simulador criado com sucesso!\n");
    return sim;
}


void simulador_destruir(Simulador *sim) {
    if (sim) {
        memoria_destruir(sim->memoria);
        for (int i = 0; i < sim->num_processos; i++) {
            processo_destruir(sim->processos[i]);
        }
        free(sim->processos);
        free(sim);
    }
}


void simulador_exibir_estatisticas(Simulador *sim) {
    if (!sim) {
        printf("Simulador não inicializado.\n");
        return;
    }
    printf("\n===== ESTATÍSTICAS DA SIMULAÇÃO =====\n");
    printf("Total de acessos à memória: %d\n", sim->total_acessos);
    printf("Total de page faults: %d\n", sim->total_page_faults);
    printf("Taxa de page fault: %.2f%%\n", 
           sim->total_acessos > 0 ? (100.0 * sim->total_page_faults / sim->total_acessos) : 0.0);
}


void simulador_exibir_memoria(Simulador *sim) {
    if (!sim || !sim->memoria) {
        printf("Simulador ou memória não inicializados.\n");
        return;
    }
    printf("\n===== ESTADO ATUAL DA MEMÓRIA =====\n");
    memoria_exibir(sim->memoria);
}

void simulador_exibir_processos(Simulador *sim) {}

Processo* simulador_adicionar_processo(Simulador *sim, int pid, int tamanho_processo) {}