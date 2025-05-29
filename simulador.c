#include "simulador.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h> 


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

// Destrói o processo (libera memória)
void simulador_processo_destruir(Simulador *sim, Processo *processo){
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

//Aloca o processo dentro da memória física
void simulador_processo_alocar_memoria(Simulador *sim, int pid, int tamanho, int tamanho_pagina) {

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

/* --- ESTRUTURA DA FUNÇÃO ---
//01. Identifica qual pagina deve ser removida com base no algoritmo de substituição selecionado.
//02. Remove a página da memória física.
//03. Adiciona a nova página.
//04. Atualiza a tabela.
*/


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


//Busca o processo dentro do frame
Processo* simulador_processo_frame(Simulador* sim, int frame){
    
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
        Processo *ProcessoNovo = simulador_processo_busca(sim, pid);
        int frameEscolhido = algoritimosSubstituicao(sim->memoria, pid, num_pagina, sim->algoritmo);
        Processo *Processo_antigo = simulador_processo_busca(sim, )
        frame = memoria_alocar_frame(sim->memoria, ProcessoNovo, Process,pid, num_pagina);

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


