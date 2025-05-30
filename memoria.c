#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define FRAME_INVALIDO -1

MemoriaFisica* memoria_criar(int tamanho_memoria_fisica, int tamanho_pagina, int numFrames) {

    MemoriaFisica *memoria = malloc(sizeof(MemoriaFisica));

    if (!memoria) {
        fprintf(stderr, "Erro ao alocar memória para a memória física.\n");
        return NULL;
    }

    memoria->num_frames = numFrames;
    memoria->frames = calloc(sizeof(Frame), numFrames);
    memoria->ultimo_frame_removido = 0;

    // Inicializa todos os frames
    for (int i = 0; i < numFrames; i++) {
        memoria->frames[i].pid = FRAME_INVALIDO; // Inicializa com -1 (sem processo)
        memoria->frames[i].num_pagina = FRAME_INVALIDO; // Inicializa com -1 (sem página)
        memoria->frames[i].referenciada = false; // Inicializa o bit R como 0
        memoria->frames[i].modificada = false; // Inicializa o bit M como 0
        memoria->frames[i].processo = NULL; // Inicializa o ponteiro do processo como NULL
    }

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
    mem->frames[frame].processo = processoNovo; // Atualiza o ponteiro do processo

    tabela_paginas_atualizar_presente(processoNovo->tabela, num_pagina, frame);

    printf("Tempo t=%d: ", mem->tempo_atual);

    return 0; // Sucesso
}

// Aloca uma página em um frame livre (retorna nº do frame ou -1)
int memoria_alocar_frame_livre(MemoriaFisica *mem, int pid, int num_pagina, Processo *processoNovo){
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
            mem->frames[i].processo = processoNovo; // Atualiza o ponteiro do processo
            printf("Tempo t=%d: ", mem->tempo_atual);
            printf("[ALOCANDO PAGINA] Carregando Página %d do Processo %d no Frame %d!\n", num_pagina, pid, i);

            return i; // Retorna o índice do frame alocado
        }
    }

    return -1; // Nenhum frame livre disponível
}


int memoria_atualizar_contador_lru(MemoriaFisica *mem){
    // Incrementa o contador LRU de todos os frames
    for (int i = 0; i < mem->num_frames; i++) {
        if (mem->frames[i].pid != FRAME_INVALIDO) {
            mem->frames[i].contador_lru++;
        }
    }

    return 0; // Sucesso
}



int algoritimosSubstituicao(MemoriaFisica *mem, int pid, int num_pagina, int algoritimo){

    int frame_escolhido = -1; 
    int maior_contador = -1;


    switch (algoritimo) {

        //First In First Out: funciona como uma fila, o primeiro a entrar é o primeiro a sair
        case 0 : //FIFO
            frame_escolhido = mem->ultimo_frame_removido; // Pega o último frame removido
            break;

        //Random: literalmente aleatorio
        case 1: //RANDOM
            frame_escolhido = rand() % mem->num_frames;
            break;

        //Least Recently Used: o que não é mais usado há mais tempo (que tem o maior "ultimo acesso")
        case 2: //LRU

            // Procura o frame com maior contador de LRU e o escolhe para substituição
            for (int i = 0; i < mem->num_frames; i++) {
                if (mem->frames[i].pid != FRAME_INVALIDO && mem->frames[i].contador_lru > maior_contador) {
                    maior_contador = mem->frames[i].contador_lru;
                    frame_escolhido = i; // Atualiza o frame escolhido
                }
            }
            break;

        //Clock: quase igual o LRU, mas usa um bit de referência para cada página
        case 3: //CLOCK;
            // Procura o primeiro frame com bit de referência 0 e o escolhe para substituição
            for (int i = 0; i < mem->num_frames; i++) {
                if (mem->frames[i].referenciada == false) {
                    frame_escolhido = i;
                    break;
                }
            }
            
            // Se não encontrar nenhum frame com bit de referência 0, escolhe o primeiro frame e reseta o bit de referência
            if (frame_escolhido == -1) {
                for (int i = 0; i < mem->num_frames; i++) {
                    if (mem->frames[i].referenciada == true) {
                        frame_escolhido = i;
                        mem->frames[i].referenciada = false;
                        break;
                    }
                }
            }
            
            // Reseta todos os demais Bits de Referência para 0
            for (int i = 0; i < mem->num_frames; i++) {
                if (i != frame_escolhido) {
                    mem->frames[i].referenciada = false;
                }
            }
            break;

    }

    return frame_escolhido;
}

//Tenta alocar um frame livre, caso não consiga, utiliza o algoritimo de substituição e aloca um frame ocupado
int memoria_alocar_frame(MemoriaFisica *mem, Processo* ProcessoNovo, Processo* ProcessoAntigo, int frame_escolhido, int pid, int num_pagina){
    int frame = memoria_alocar_frame_livre(mem, pid, num_pagina, ProcessoNovo);
    
    // Atualiza a tabela de páginas se conseguiu alocar em frame livre
    if (frame != -1) {
        tabela_paginas_atualizar_presente(ProcessoNovo->tabela, num_pagina, frame);
        return frame;
    }

    // Nenhum frame livre encontrado, tenta substituir
    frame = memoria_alocar_frame_ocupado(mem, ProcessoNovo, ProcessoAntigo, pid, num_pagina, frame_escolhido);
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
    mem->ultimo_frame_removido = (mem->ultimo_frame_removido+1) % mem->num_frames; // Atualiza o último frame removido
}
