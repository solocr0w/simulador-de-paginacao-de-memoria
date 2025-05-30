#include "simulador.h"  
#include <stdio.h>      
#include <stdlib.h>
#include <limits.h>
#include "memoria.h"    
#include "processo.h"


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
            free(sim->processos[i]->tabela->paginas); // Libera as páginas de cada processo
            free(sim->processos[i]->tabela); // Libera a tabela de páginas de cada processo
            free(sim->processos[i]); // Libera o próprio processo
        }
        sim->tempo_sistema = 0;
        sim->tempo_total_sistema = 0;
        free(sim->processos);
        free(sim);
    }
}



//Aloca o processo dentro da memória física
void simulador_processo_alocar_memoria(Simulador *sim, int pid, int tamanho, int tamanho_pagina) {

    Processo *processo = simulador_processo_busca(sim, pid);

    if (!processo || !processo->tabela) {
        fprintf(stderr, "Processo ou tabela de páginas inválidos.\n");
        return;
    }

    // Aloca as páginas na memória física
    for (int i = 0; i < processo->num_paginas; i++) {
        Pagina *pagina = &processo->tabela->paginas[i];
        pagina->frame = memoria_alocar_frame_livre(sim->memoria, pid, i, processo);
        pagina->presente = (pagina->frame != -1);
        pagina->modificada = false;
        pagina->referenciada = false;
        pagina->ultimo_acesso = pagina->presente ? sim->tempo_sistema : 0; 
    }
    
}

Processo* simulador_processo_frame(Simulador* sim, int frame){
    return sim->memoria->frames[frame].processo;
}


// Busca por um processo pelo PID
Processo* simulador_processo_busca(Simulador *sim, int pid){
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

// E o prémio de melhor função vai para...
void simulador_exibir_memoria(Simulador *sim) {
    memoria_exibir(sim->memoria);
}

Processo *simulador_adicionar_processo(Simulador *sim, int tamanho_processo)
{

    if (!sim)
    {
        printf("Simulador não inicializado.\n");
        return NULL;
    }


    int pid;

    if (sim->num_processos == 0)
    {
        pid = 0;
    }
    else
    {
        pid = sim->num_processos;
        for (int i = 0; i < sim->num_processos; i++)
        {
            if (sim->processos[i]->pid == pid)
            {
                pid++;
                i = -1; 
            }
        }
    }

    // Cria um novo processo
    Processo *novo_processo = processo_criar(pid, tamanho_processo, sim->tamanho_pagina);
    if (!novo_processo)
    {
        printf("Erro ao criar o processo.\n");
        return NULL;
    }

    // Adiciona o processo ao array de processos
    sim->processos = realloc(sim->processos, sizeof(Processo *) * (sim->num_processos + 1));
    sim->processos[sim->num_processos] = novo_processo;
    sim->num_processos++;

    return novo_processo;
}

int simulador_acessar_memoria(Simulador *sim, int pid, int endereco_virtual){
    if (!sim || !sim->memoria) {
        printf("Simulador ou memória não inicializados.\n");
        return -1;
    }

    Processo *processo = simulador_processo_busca(sim, pid);
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
        Processo *ProcessoNovo = simulador_processo_busca(sim, pid);
        int frameEscolhido = algoritimosSubstituicao(sim->memoria, pid, num_pagina, sim->algoritmo);
        Processo *Processo_antigo = simulador_processo_frame(sim, frameEscolhido);
        frame = memoria_alocar_frame(sim->memoria, ProcessoNovo, Processo_antigo, frameEscolhido, pid, num_pagina);

        if (frame == -1) {
            printf("Erro ao alocar frame para o processo %d.\n", pid);
            return -1; 
        }
        
        tabela_paginas_atualizar_presente(processo->tabela, num_pagina, frame);

    } else {
        int endereco_fisico = frame * sim->tamanho_pagina + deslocamento;

        printf("Tempo t=%d: ", sim->tempo_sistema);
        printf("[PAGE HIT] Endereço virtual (P%d): %d -> Página: %d -> Frame: %d -> Endereço físico: %d\n", pid, endereco_virtual, num_pagina, frame, endereco_fisico);
        printf("\n");

        sim->memoria->frames[frame].contador_lru = 0;
        sim->memoria->frames[frame].referenciada = true;

        processo->tabela->paginas[num_pagina].ultimo_acesso = sim->tempo_sistema;
    }

    sim->total_acessos++;
    return 0;
}

void loopSimulador(Simulador *sim) {

    for(int i = 0; i < sim->total_processos; i++) {
        Processo *processo = simulador_adicionar_processo(sim, sim->tamanho_processos[i]);
    }


    while(sim->tempo_sistema < sim->tempo_total_sistema){

        // Simula um acesso à memória virtual de um processo
        Processo *processo = sim->processos[rand() % sim->num_processos]; 

        int endereco_virtual = rand() % processo->tamanho; 
        simulador_acessar_memoria(sim, processo->pid, endereco_virtual);

        // Exibe o estado atual da memória física
        simulador_exibir_memoria(sim);

        memoria_atualizar_contador_lru(sim->memoria);

        sim->tempo_sistema++;
        sim->memoria->tempo_atual++; 
    }

    simulador_exibir_estatisticas(sim);

    simulador_destruir(sim);
};


