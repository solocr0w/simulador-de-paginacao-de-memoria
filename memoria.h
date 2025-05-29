#ifndef MEMORIA_H
#define MEMORIA_H
#define FRAME_INVALIDO -1

#include <stdbool.h>
#include "processo.h"
#include <limits.h>


typedef struct {
    int pid;            // ID do processo dono do frame
    int num_pagina;     // Número da página alocada neste frame
    bool referenciada;  // Bit R (para Clock/Second Chance)
    bool modificada;    // Bit M (para substituição)
    int tempo_carga;    // Instante em que cada frame foi carregado (para FIFO)
} Frame;

typedef struct {
    Frame *frames;                     // Array de frames (cada frame armazena PID + nº página)
    int num_frames;                  // Número total de frames na memória
    int tempo_atual;                 // Relógio global da simulação (opcional, pode ser usado para FIFO)   
} MemoriaFisica;

MemoriaFisica* memoria_criar(int tamanho_memoria_fisica, int tamanho_pagina, int numFrames);

void memoria_destruir(MemoriaFisica *mem);

// Aloca uma página em um frame livre (retorna nº do frame ou -1)
int memoria_alocar_frame_livre(MemoriaFisica *mem, int pid, int num_pagina);

// Aloca uma página em um frame específico (retorna 0 se sucesso, -1 se falha)
int memoria_alocar_frame_ocupado(MemoriaFisica *mem, Processo *processoNovo, Processo *ProcessoAntigo,int pid, int num_pagina, int frame);

//Tenta alocar um frame livre, caso não consiga, utiliza o forame escolhido o frame mesmo ocupado
int memoria_alocar_frame(MemoriaFisica *mem, Processo* ProcessoNovo, Processo* ProcessoAntigo, int frame_escolhido, int pid, int num_pagina);

// Libera um frame específico
void memoria_liberar_frame(MemoriaFisica *mem, int frame);

// Busca um frame que contém a página (pid, num_pagina) (retorna -1 se não encontrado)
int memoria_buscar_frame(MemoriaFisica *mem, int pid, int num_pagina);

// Exibe o estado atual da memória física (para debug/simulação)
void memoria_exibir(MemoriaFisica *mem);

// Remove uma página do frame específico 
void removerFrame(MemoriaFisica *mem, Processo *processoDono, int frame_id);

// Implementa o algoritmo de substituição de páginas (FIFO, LRU, etc.)
int algoritimosSubstituicao(MemoriaFisica *mem, int pid, int num_pagina, int algoritmo);

#endif