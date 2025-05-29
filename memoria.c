#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"
#include "tabela-pagina.h"
#include "processo.h"
#include "simulador.h"
#include <stdbool.h>


#define FRAME_INVALIDO -1

// TODO: Desenvolver as funções de alocação, liberação e busca de frames
MemoriaFisica* memoria_criar(int tamanho_memoria_fisica, int tamanho_pagina, int numFrames) {

    MemoriaFisica *memoria = malloc(sizeof(MemoriaFisica));

    if (!memoria) {
        fprintf(stderr, "Erro ao alocar memória para a memória física.\n");
        return NULL;
    }

    memoria->num_frames = numFrames;
    memoria->frames = calloc(sizeof(Frame), numFrames);
    
    if (!memoria->frames) {
        free(memoria);
        fprintf(stderr, "Erro ao alocar memória para os frames.\n");
        return NULL;
    }

    // Inicializa todos os frames
    for (int i = 0; i < numFrames; i++) {
        memoria->frames[i].pid = FRAME_INVALIDO; // Inicializa com -1 (sem processo)
        memoria->frames[i].num_pagina = FRAME_INVALIDO; // Inicializa com -1 (sem página)
        memoria->frames[i].tempo_carga = FRAME_INVALIDO; // Inicializa com -1 (nunca carregado)
        memoria->frames[i].referenciada = false; // Inicializa o bit R como 0
        memoria->frames[i].modificada = false; // Inicializa o bit M como 0
    }

    printf("Memória física criada com sucesso!\n");
    return memoria;
}

void memoria_destruir(MemoriaFisica *mem) {
    if (mem) {
        free(mem->frames);
        free(mem);
    }
}

// Aloca uma página em um frame específico, mesmo que ja esteja ocupado (retorna 0 se sucesso, -1 se falha)
int memoria_alocar_frame_ocupado(Simulador *sim, int pid, int num_pagina, int frame){
    if (!sim || !sim->memoria || frame < 0 || frame >= sim->memoria->num_frames) {
        fprintf(stderr, "Memória física não inicializada ou frame inválido.\n");
        return -1;
    }

    removerFrame(sim, frame); // Remove o frame antes de alocar

    // Aloca o frame com os dados do processo
    sim->memoria->frames[frame].pid = pid;
    sim->memoria->frames[frame].num_pagina = num_pagina;
    sim->memoria->frames[frame].referenciada = false;
    sim->memoria->frames[frame].modificada = false;
    sim->memoria->frames[frame].tempo_carga = sim->tempo_sistema; // Atualiza o tempo de carga com o tempo do sistema

    // Atualiza a tabela de páginas do processo
    Processo *processo = processo_busca(sim, pid);
    tabela_paginas_atualizar_presente(processo->tabela, num_pagina, frame);

    printf("Tempo t=%d: ", sim->tempo_sistema);
    printf("[SUBSTITUIÇÃO] Substituindo Página %d do Processo %d no Frame %d pela Página %d do Processo %d!\n", num_pagina, pid, frame, num_pagina);

    return 0; // Sucesso
}

// Aloca uma página em um frame livre (retorna nº do frame ou -1)
int memoria_alocar_frame_livre(Simulador *sim, int pid, int num_pagina){
    if (!sim || !sim->memoria) {
        fprintf(stderr, "Memória física não inicializada.\n");
        return -1;
    }

    // Busca um frame livre
    for (int i = 0; i < sim->memoria->num_frames; i++) {
        if (sim->memoria->frames[i].pid == FRAME_INVALIDO) {
            // Encontra um frame livre
            sim->memoria->frames[i].pid = pid;
            sim->memoria->frames[i].num_pagina = num_pagina;
            sim->memoria->frames[i].referenciada = false;
            sim->memoria->frames[i].modificada = false;
            sim->memoria->frames[i].tempo_carga = 0; // Pode ser atualizado com o tempo do sistema

            printf("Tempo t=%d: ", sim->tempo_sistema);
            printf("[ALOCANDO PAGINA] Carregando Página %d do Processo %d no Frame %d!\n", num_pagina, pid, i);

            return i; // Retorna o índice do frame alocado
        }
    }

    return -1; // Nenhum frame livre disponível
}


//Tenta alocar um frame livre, caso não consiga, utiliza o algoritimo de substituição e aloca um frame ocupado
int memoria_alocar_frame(Simulador *sim, int pid, int num_pagina){
    int frame = memoria_alocar_frame_livre(sim, pid, num_pagina);
    
    // Nenhum frame livre encontrado, tenta substituir
    if (frame == -1) {

        //Chama o algoritimo de substituição
        int frame_escolhido = algoritimosSubstituicao(sim, pid, num_pagina);

        //Aloca o frame escolhido na memoria fisica
        frame = memoria_alocar_frame_ocupado(sim, pid, num_pagina, frame_escolhido);
    }
    return frame;
}

// Libera um frame específico
void memoria_liberar_frame(MemoriaFisica *mem, int frame);

// Busca um frame que contém a página (pid, num_pagina) (retorna -1 se não encontrado)
int memoria_buscar_frame(Simulador *sim, int pid, int num_pagina){
  
    MemoriaFisica *mem = sim->memoria;

    for (int i = 0; i < mem->num_frames; i++) {
        if (mem->frames[i].pid == pid && mem->frames[i].num_pagina == num_pagina) {

            mem->frames[i].referenciada = true; // Marca como referenciada
            return i; // Retorna o índice do frame encontrado
        }
    }
    return -1; // Nenhum frame encontrado
}

// Exibe o estado atual da memória física (para debug/simulação)
void memoria_exibir(MemoriaFisica *mem);

void removerFrame(Simulador *sim, int frame_id) {

    Frame *frame = &sim->memoria->frames[frame_id];
    
    // Se o frame já estiver livre, nada a fazer
    if (frame->pid == -1) return;
    int pid_dono = frame->pid;
    int pagina_dona = frame->num_pagina;

    //Atualiza a tabela de páginas
    tabela_paginas_atualizar_nao_presente(sim->processos[pid_dono]->tabela, pagina_dona);

    //Limpa o frame, removendo-o
    frame->pid = -1;
    frame->num_pagina = -1;
    frame->referenciada = false;
    frame->modificada = false;
    frame->tempo_carga = 0;
}
