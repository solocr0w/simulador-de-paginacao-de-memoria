#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"
#include "memoria.h"
#include "processo.h"
#include "tabela-pagina.h"

//TODO: 
// 1. Implementar as função simulador_adicionar_processo
// 2. Implementar a função simulador_exibir_processos
// 3. Implementar a função simulador_exibir_memoria


Simulador* criarSimulador(int tamanho_pagina, int tamanho_memoria_fisica, AlgoritmoSubstituicao algoritmoEscolhido, int numFrames) {
    Simulador *sim = malloc(sizeof(Simulador));
    // if (!sim) {
    //     fprintf(stderr, "Erro ao alocar memória para o simulador.\n");
    //     return NULL;
    // }

    sim->tamanho_pagina = tamanho_pagina;
    sim->tamanho_memoria_fisica = tamanho_memoria_fisica;
    sim->algoritmo = algoritmoEscolhido;
    sim->memoria = memoria_criar(tamanho_memoria_fisica, tamanho_pagina, numFrames);
    // if (!sim->memoria) {
    //     free(sim);
    //     fprintf(stderr, "Erro ao criar memória física.\n");
    //     return NULL;
    // }
    sim->processos = malloc(sizeof(Processo*) * numFrames); //TODO: REVER SE ISSO FAZ SENTIDO
    // if (!sim->processos) {
    //     memoria_destruir(sim->memoria);
    //     free(sim);
    //     fprintf(stderr, "Erro ao alocar memória para os processos.\n");
    //     return NULL;
    // }
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
            //processo_destruir(sim->processos[i]);
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
    //memoria_exibir(sim->memoria);
}

void simulador_exibir_processos(Simulador *sim) {}

int simulador_adicionar_processo(Simulador *sim, int pid, int tamanho_processo) {}

void loopSimulador(Simulador *sim) {
    if (!sim) {
        printf("Simulador não inicializado.\n");
        return;
    }
    
    int opcao;
    do {
        printf("\n===== MENU DO SIMULADOR =====\n");
        printf("1. Criar um novo processo.\n");
        printf("2. Exibir estado da memória física\n");
        printf("3. Exibir tabela de processos\n");
        printf("4. Exibir estatísticas\n");
        printf("5. Sair\n");
        printf("Escolha uma opção: ");

        scanf("%d", &opcao);
        if (opcao < 1 || opcao > 5) {
            printf("Opção inválida. Tente novamente.\n");
            continue;
        }
    
        switch(opcao) {
            case 1:
                // Gerei esse filler mas na real aqui vai ficar a função de gerar processos
                printf("Criando um novo processo...\n");
                int pid, tamanho_processo;
                printf("Digite o PID do processo: ");
                scanf("%d", &pid);
                printf("Digite o tamanho do processo em bytes: ");
                scanf("%d", &tamanho_processo);
                simulador_adicionar_processo(sim, pid, tamanho_processo);
                break;
            case 2:
                //memoria_exibir(sim->memoria);
                break;
            case 3:
                // Será que vale a pena fazer?
                break;
            case 4:
                simulador_exibir_estatisticas(sim);
                break;
            case 5:
                printf("Saindo do simulador...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 5);
};
