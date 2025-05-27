#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"
#include "tabela-pagina.h"
#include "processo.h"
#include <stdbool.h>


#define FRAME_INVALIDO -1

// TODO: Desenvolver as funções de alocação, liberação e busca de frames

MemoriaFisica* memoria_criar(int tamanho_memoria_fisica, int tamanho_pagina, int numFrames) {
    MemoriaFisica *memoria = malloc(sizeof(MemoriaFisica));
    if (!memoria) {
        fprintf(stderr, "Erro ao alocar memória para a memória física.\n");
        return NULL;
    }
    memoria->tamanho_memoria = tamanho_memoria_fisica;
    memoria->tamanho_pagina = tamanho_pagina;
    memoria->num_frames = numFrames;
    memoria->frames = malloc(sizeof(Frame) * numFrames);
    if (!memoria->frames) {
        free(memoria);
        fprintf(stderr, "Erro ao alocar memória para os frames.\n");
        return NULL;
    }
    memoria->tempo_carga = malloc(sizeof(int) * numFrames);
    for (int i = 0; i < numFrames; i++) {
        memoria->tempo_carga[i] = -1; // Inicializa com -1 (nunca carregado)
    }
    for (int i = 0; i < numFrames; i++) {
        memoria->frames[i] = FRAME_INVALIDO; // Inicializa todos os frames como inválidos
    }
    memoria->referenciada = malloc(sizeof(bool) * numFrames);
    for (int i = 0; i < numFrames; i++) {
        memoria->referenciada[i] = false; // Inicializa todos os bits R como 0
    }
    memoria->modificada = malloc(sizeof(bool) * numFrames);
    for (int i = 0; i < numFrames; i++) {
        memoria->modificada[i] = false; // Inicializa todos os bits M como 0
    }
    
    printf("Memória física criada com sucesso!\n");
    return memoria;
}

void memoria_destruir(MemoriaFisica *mem) {
    if (mem) {
        free(mem->frames);
        free(mem->tempo_carga);
        free(mem->referenciada);
        free(mem->modificada);
        free(mem);
    }
}


// Aloca uma página em um frame livre (retorna nº do frame ou -1)
int memoria_alocar_frame(MemoriaFisica *mem, int pid, int num_pagina);

// Libera um frame específico
void memoria_liberar_frame(MemoriaFisica *mem, int frame);

// Busca um frame que contém a página (pid, num_pagina) (retorna -1 se não encontrado)
int memoria_buscar_frame(MemoriaFisica *mem, int pid, int num_pagina);

// Exibe o estado atual da memória física (para debug/simulação)
void memoria_exibir(MemoriaFisica *mem);
