#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"
#include "memoria.h"
#include "processo.h"
#include "tabela-pagina.h"
#include "pagina.h"
#include <limits.h> //INT_MAX

//TODO: 
// 1. Implementar as função simulador_adicionar_processo
// 2. Implementar a função simulador_exibir_processos
// 3. Implementar a função simulador_exibir_memoria


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

    //Não existe um array de processos, mas sim um ponteiro para ponteiro de processos
    sim->processos = NULL;
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

/* --- ESTRUTURA DA FUNÇÃO ---
//01. Identifica qual pagina deve ser removida com base no algoritmo de substituição selecionado.
//02. Remove a página da memória física.
//03. Adiciona a nova página.
//04. Atualiza a tabela.
*/
int algoritimosSubstituicao(Simulador *sim, int pid, int num_pagina){

    int frame_escolhido = -1; 

    //onde que a gente verifica se tem frame livre? sera q nao vale implementar isso em memoria?

    switch (sim->algoritmo) {

        //First In First Out: funciona como uma fila, o primeiro a entrar é o primeiro a sair
        case FIFO:
            printf("Substituição FIFO selecionada.\n");
            int mais_antigo = INT_MAX;
                for (int i = 0; i < sim->memoria->num_frames; i++) {

                    //compara o tempo de carga e, se for menor que o atual "mais antigo", atualiza
                    if (sim->memoria->frames[i].tempo_carga < mais_antigo) {
                        mais_antigo = sim->memoria->frames[i].tempo_carga;
                        frame_escolhido = i;
                    }
                }
            break;

        //Random: literalmente aleatorio
        case RANDOM:
            printf("Substituição aleatória selecionada.\n");
            frame_escolhido = rand() % sim->memoria->num_frames;
            break;

        //Least Recently Used: o que não é mais usado há mais tempo (que tem o maior "ultimo acesso")
        case LRU:
            printf("Substituição LRU selecionada.\n");
            break;

        //Clock: quase igual o LRU, mas usa um bit de referência para cada página
        case CLOCK:
            printf("Substituição Clock selecionada.\n");
            break;
    }

    return frame_escolhido;
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

Processo* simulador_adicionar_processo(Simulador *sim, int tamanho_processo){

    if (!sim) {
        printf("Simulador não inicializado.\n");
        return NULL;
    }

    //Escolhendo o PID do novo processo
    int pid = sim->num_processos;

    // Cria um novo processo
    Processo *novo_processo = processo_criar(pid, tamanho_processo, sim->tamanho_pagina);
    if (!novo_processo) {
        printf("Erro ao criar o processo.\n");
        return NULL;
    }

    // Adiciona o processo ao array de processos
        sim->processos = realloc(sim->processos, sizeof(Processo*) * (sim->num_processos + 1));
        if (!sim->processos) {
            printf("Erro ao alocar memória para o array de processos.\n");
            processo_destruir(novo_processo);
            return NULL;
        }    
    
    
    sim->processos[sim->num_processos] = novo_processo;
    sim->num_processos++;
    
    return novo_processo;
}

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
                Processo *novo_processo = simulador_adicionar_processo(sim, tamanho_processo);
                if (novo_processo) {
                    printf("Processo %d criado com sucesso!\n", novo_processo->pid);
                } else {
                    printf("Erro ao criar o processo.\n");
                }
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
