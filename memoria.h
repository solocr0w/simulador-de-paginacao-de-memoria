#ifndef MEMORIA_H
#define MEMORIA_H

#include "tabela-pagina.h"

#define FRAME_INVALIDO -1

typedef struct {
    int tamanho_memoria;             // Tamanho total da memória física em bytes
    int tamanho_pagina;              // Tamanho de cada página em bytes
    int num_frames;                  // Número total de frames na memória
    int *frames;                     // Array de frames (cada frame armazena PID + nº página)
    int *tempo_carga;                // Instante em que cada frame foi carregado (para FIFO)
    bool *referenciada;               // Bit R (para Clock/Second Chance)
    bool *modificada;                 // Bit M (para substituição)
    int tempo_atual;                  // Relógio global da simulação
    
} MemoriaFisica;

// Cria uma memória física com 'num_frames' frames
MemoriaFisica* memoria_criar(int tamanho_memoria, int tamanho_pagina, int num_frames);

// Destrói a memória física
void memoria_destruir(MemoriaFisica *mem);

// Aloca uma página em um frame livre (retorna nº do frame ou -1)
int memoria_alocar_frame(MemoriaFisica *mem, int pid, int num_pagina);

// Libera um frame específico
void memoria_liberar_frame(MemoriaFisica *mem, int frame);

// Busca um frame que contém a página (pid, num_pagina) (retorna -1 se não encontrado)
int memoria_buscar_frame(MemoriaFisica *mem, int pid, int num_pagina);

// Exibe o estado atual da memória física (para debug/simulação)
void memoria_exibir(MemoriaFisica *mem);

#endif