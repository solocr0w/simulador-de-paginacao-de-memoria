#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"
#include "memoria.h"
#include "processo.h"
#include "tabela-pagina.h"
#include "pagina.h"
#include <limits.h> //INT_MAX


Simulador* criarSimulador(int tamanho_pagina, int tamanho_memoria_fisica, AlgoritmoSubstituicao algoritmoEscolhido, int numFrames, int total_processos, int *tamanho_processos, int tempo_total_sistema) {

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

    sim->total_processos = total_processos;
    sim->tamanho_processos = tamanho_processos;
    sim->tempo_total_sistema = tempo_total_sistema;

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

Processo* simulador_adicionar_processo(Simulador *sim){

    if (!sim) {
        printf("Simulador não inicializado.\n");
        return NULL;
    }

    //Encontra um tamanho de processo com até 10 páginas
    int tamanho_processo = (rand() % 10 + 1) * sim->tamanho_pagina; // Gera um tamanho entre 1 e 10 páginas

    //Escolhendo o PID do novo processo
    int pid;

    //Escolhe um PID único para o novo processo
    if (sim->num_processos == 0) {
        pid = 0; // Primeiro processo sempre tem PID 0
    } else {
        // Encontra o próximo PID disponível
        pid = sim->num_processos;
        for (int i = 0; i < sim->num_processos; i++) {
            if (sim->processos[i]->pid == pid) {
                pid++;
                i = -1; // Reinicia a busca se o PID já estiver em uso
            }
        }
    }

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

int simulador_acessar_memoria(Simulador *sim, int pid, int endereco_virtual){
    if (!sim || !sim->memoria) {
        printf("Simulador ou memória não inicializados.\n");
        return -1;
    }

    Processo *processo = processo_busca(sim, pid);
    if (!processo) {
        printf("Processo com PID %d não encontrado.\n", pid);
        return -1;
    }

    int num_pagina, deslocamento;
    processo_extrair_endereco(processo, endereco_virtual, &num_pagina, &deslocamento, sim->tamanho_pagina);

    // Verifica se a página está presente na memória
    int frame = memoria_buscar_frame(sim->memoria, pid, num_pagina);
    
    
    if (frame == -1) {
        // Page fault: a página não está na memória
        printf("Tempo t=%d: ", sim->tempo_sistema);
        printf("[PAGE FAULT] Página %d do processo %d não está na memória física!\n", num_pagina, pid);
        sim->total_page_faults++;
        
        // Aloca um frame para a página
        frame = memoria_alocar_frame(sim, pid, num_pagina);
        if (frame == -1) {
            printf("Erro ao alocar frame para o processo %d.\n", pid);
            return -1; // Falha ao alocar frame
        }
        
        // Atualiza a tabela de páginas do processo
        tabela_paginas_atualizar_presente(processo->tabela, num_pagina, frame);

    } else {
        // Página já está na memória, atualiza o último acesso
        int endereco_fisico = frame * sim->tamanho_pagina + deslocamento;

        printf("Tempo t=%d: ", sim->tempo_sistema);
        printf("[BUSCA FRAME] Endereço virtual (P%d): %d -> Página: %d -> Frame: %d -> Endereço físico: %d\n", pid, endereco_virtual, num_pagina, frame, endereco_fisico);

        processo->tabela->paginas[num_pagina].ultimo_acesso = sim->tempo_sistema;
    }

    sim->total_acessos++;
    return 0; // Sucesso
}

void loopSimulador(Simulador *sim) {
    if (!sim) {
        printf("Simulador não inicializado.\n");
        return;
    }

    while(sim->tempo_sistema < sim->tempo_total_sistema){

        // Gerei esse filler mas na real aqui vai ficar a função de gerar processos
        printf("Criando um novo processo...\n");
    
    Processo *novo_processo = simulador_adicionar_processo(sim);

    if (!novo_processo)
        printf("Erro ao criar o processo.\n");

        // Simula um acesso à memória virtual de um processo
        Processo *processo = sim->processos[rand() % sim->num_processos]; // Seleciona um processo aleatório

        // Gera um endereço virtual aleatório dentro do tamanho do processo
        int endereco_virtual = rand() % processo->tamanho; // Gera um endereço virtual aleatório
        simulador_acessar_memoria(sim, processo->pid, endereco_virtual);

        // Exibe o estado atual da memória física
        simulador_exibir_memoria(sim);

        sim->tempo_sistema++; // Incrementa o tempo do sistema a cada iteração
    }

    // Exibe as estatísticas da simulação
    simulador_exibir_estatisticas(sim);

    // Destroi o simulador
    simulador_destruir(sim);

};


