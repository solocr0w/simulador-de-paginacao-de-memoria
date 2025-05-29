#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
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
    memoria->ultimo_frame_removido = 0;
    if (!memoria->frames) {
        free(memoria);
        fprintf(stderr, "Erro ao alocar memória para os frames.\n");
        return NULL;
    }

    // Inicializa todos os frames
    for (int i = 0; i < numFrames; i++) {
        memoria->frames[i].pid = FRAME_INVALIDO; // Inicializa com -1 (sem processo)
        memoria->frames[i].num_pagina = FRAME_INVALIDO; // Inicializa com -1 (sem página)
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
int memoria_alocar_frame_ocupado(MemoriaFisica *mem, Processo *processoNovo, Processo *ProcessoAntigo,int pid, int num_pagina, int frame){

    removerFrame(mem, ProcessoAntigo, frame); // Remove o processo do frame antes de alocar

    // Aloca o frame com os dados do processo
    mem->frames[frame].pid = pid;
    mem->frames[frame].num_pagina = num_pagina;
    mem->frames[frame].referenciada = false;
    mem->frames[frame].modificada = false;

    tabela_paginas_atualizar_presente(processoNovo->tabela, num_pagina, frame);

    printf("Tempo t=%d: ", mem->tempo_atual);
    //TODO: printf("[SUBSTITUIÇÃO] Substituindo Página %d do Processo %d no Frame %d pela Página %d do Processo %d!\n", num_pagina, pid, frame, num_pagina);

    return 0; // Sucesso
}

// Aloca uma página em um frame livre (retorna nº do frame ou -1)
int memoria_alocar_frame_livre(MemoriaFisica *mem, int pid, int num_pagina){
    if (!mem) {
        fprintf(stderr, "Memória física não inicializada.\n");
        return -1;
    }

    // Busca um frame livre
    for (int i = 0; i < mem->num_frames; i++) {
        if (mem->frames[i].pid == FRAME_INVALIDO) {
            // Encontra um frame livre
            mem->frames[i].pid = pid;
            mem->frames[i].num_pagina = num_pagina;
            mem->frames[i].referenciada = false;
            mem->frames[i].modificada = false;
            printf("Tempo t=%d: ", mem->tempo_atual);
            printf("[ALOCANDO PAGINA] Carregando Página %d do Processo %d no Frame %d!\n", num_pagina, pid, i);

            return i; // Retorna o índice do frame alocado
        }
    }

    return -1; // Nenhum frame livre disponível
}


int algoritimosSubstituicao(MemoriaFisica *mem, int pid, int num_pagina, int algoritimo){

    int frame_escolhido = -1; 

    //onde que a gente verifica se tem frame livre? sera q nao vale implementar isso em memoria?

    switch (algoritimo) {

        //First In First Out: funciona como uma fila, o primeiro a entrar é o primeiro a sair
        case 0 : //FIFO
            printf("Substituição FIFO selecionada.\n");
            
            break;

        //Random: literalmente aleatorio
        case 1: //RANDOM
            printf("Substituição aleatória selecionada.\n");
            frame_escolhido = rand() % mem->num_frames;
            break;

        //Least Recently Used: o que não é mais usado há mais tempo (que tem o maior "ultimo acesso")
        case 2: //LRU
            printf("Substituição LRU selecionada.\n");
            break;

        //Clock: quase igual o LRU, mas usa um bit de referência para cada página
        case 3: //CLOCK
            printf("Substituição Clock selecionada.\n");
            break;
    }

    return frame_escolhido;
}

//Tenta alocar um frame livre, caso não consiga, utiliza o algoritimo de substituição e aloca um frame ocupado
int memoria_alocar_frame(MemoriaFisica *mem, Processo* ProcessoNovo, Processo* ProcessoAntigo, int frame_escolhido, int pid, int num_pagina){
    int frame = memoria_alocar_frame_livre(mem, pid, num_pagina);
    
    // Nenhum frame livre encontrado, tenta substituir
    if (frame == -1) {

        //Aloca o frame escolhido na memoria fisica
        frame = memoria_alocar_frame_ocupado(mem, ProcessoNovo, ProcessoAntigo, pid, num_pagina, frame_escolhido);
    }
    return frame;
}


// Busca um frame que contém a página (pid, num_pagina) (retorna -1 se não encontrado)
int memoria_buscar_frame(MemoriaFisica *mem, int pid, int num_pagina){

    for (int i = 0; i < mem->num_frames; i++) {
        if (mem->frames[i].pid == pid && mem->frames[i].num_pagina == num_pagina) {

            mem->frames[i].referenciada = true; // Marca como referenciada
            return i; // Retorna o índice do frame encontrado
        }
    }
    return -1; // Nenhum frame encontrado
}

// Exibe o estado atual da memória física (para debug/simulação)
void memoria_exibir(MemoriaFisica *mem){
    // Representando a memória física como uma tabela
    printf("Estado atual da memória física:\n");
    printf("Frame\tPID\tPágina\n");
    for (int i = 0; i < mem->num_frames; i++) {
        Frame *frame = &mem->frames[i];
        printf("%d\t%d\t%d\n", 
               i, 
               frame->pid, 
               frame->num_pagina
               );
    }
    printf("\n");

}

void removerFrame(MemoriaFisica *mem, Processo *processoDono, int frame_id) {

    Frame *frame = &mem->frames[frame_id];
    
    // Se o frame já estiver livre, nada a fazer
    if (frame->pid == -1) return;
    int pid_dono = frame->pid;
    int pagina_dona = frame->num_pagina;

    //Atualiza a tabela de páginas
    tabela_paginas_atualizar_nao_presente(processoDono->tabela, pagina_dona);

    //Limpa o frame, removendo-o
    frame->pid = -1;
    frame->num_pagina = -1;
    frame->referenciada = false;
    frame->modificada = false;
}
