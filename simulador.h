#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "processo.h"
#include "memoria.h"

// Define os algoritmos de substituição suportados
typedef enum {
    FIFO,
    RANDOM,
    LRU,
    CLOCK

} AlgoritmoSubstituicao;


// Estrutura principal do simulador
typedef struct {
    //Vou tirar isso por enquanto para nn atrapalhar :int tempo_atual;                 // Relógio global da simulação
    int tamanho_pagina;              // Tamanho da página em bytes (ex: 4096)
    int tamanho_memoria_fisica;      // Tamanho total da memória física em bytes
    AlgoritmoSubstituicao algoritmo; // Algoritmo de substituição selecionado
    MemoriaFisica *memoria;          // Memória física do sistema
    Processo **processos;            // Array de processos ativos
    int num_processos;               // Número de processos alocados

    // Estatísticas
    int total_acessos;               // Total de acessos à memória simulados
    int total_page_faults;           // Total de page faults ocorridos

} Simulador;


// Funções principais
Simulador* criarSimulador(int tamanho_pagina, int tamanho_memoria_fisica, AlgoritmoSubstituicao algoritmoEscolhido, int numFrames);
   
// Input Listening para a simulação, onde será executado o menu de simulação
void loopSimulador(Simulador *sim);


void simulador_destruir(Simulador *sim);

// Adiciona um novo processo ao simulador
Processo* simulador_adicionar_processo(Simulador *sim, int pid, int tamanho_processo);

// Simula um acesso à memória virtual de um processo
int simulador_acessar_memoria(Simulador *sim, int pid, int endereco_virtual);

// Exibe o estado atual do simulador
//void simulador_exibir_estado(Simulador *sim);

#endif
