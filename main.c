#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"
#include "memoria.h"

int lerInteiro(const char* prompt) {
    char buffer[100];
    int valor;
    
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Erro ao ler entrada.\n");
            continue;
        }
        
        if (sscanf(buffer, "%d", &valor) != 1) {
            printf("Entrada inválida. Digite apenas números inteiros.\n");
            continue;
        }
        return valor;
    }
}


void novaSimulacao() {
    int tamanhoPagina, tamanhoMemoria, numFrames, algoritmoNum, totalProcessos, tempo_total_sistema;
    printf("\n===== SIMULADOR DE PAGINAÇÃO =====\n");
    
    tamanhoPagina = lerInteiro("Digite o tamanho da página em bytes: ");
    tamanhoMemoria = lerInteiro("Digite o tamanho da memória física em bytes: ");
    tempo_total_sistema = lerInteiro("Digite o tempo total do sistema (em ciclos): ");
    totalProcessos = lerInteiro("Digite o número total de processos: ");
    int *tamanhoProcessos = malloc(totalProcessos * sizeof(int));
    if (!tamanhoProcessos) {
        fprintf(stderr, "Erro ao alocar memória para os tamanhos dos processos.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < totalProcessos; i++) {
        printf("Processo %d: ", i);
        tamanhoProcessos[i] = lerInteiro("Digite o tamanho do processo em bytes: ");
    }
    numFrames = tamanhoMemoria / tamanhoPagina;
    
    printf("Selecione o algoritmo de substituição:\n");
    printf("1-FIFO\n2-LRU\n3-CLOCK\n4-RANDOM\n");
    algoritmoNum = lerInteiro("Número do Algoritmo: ");
    
    printf("\n===== CONFIGURAÇÃO DA SIMULAÇÃO =====\n");
    printf("Tamanho da página: %d bytes (%d KB)\n", tamanhoPagina, tamanhoPagina/1024);
    printf("Tamanho da memória física: %d bytes (%d KB)\n", tamanhoMemoria, tamanhoMemoria/1024);
    printf("Número de frames: %d\n", numFrames);
    printf("Algoritmo de substituição: ");
    AlgoritmoSubstituicao algoritmo;
    switch(algoritmoNum) {
        case 1: printf("FIFO\n"); algoritmo=FIFO; break;
        case 2: printf("LRU\n"); algoritmo=LRU; break;
        case 3: printf("CLOCK\n"); algoritmo=CLOCK; break;
        case 4: printf("RANDOM\n"); algoritmo=RANDOM; break;
        default: 
            printf("Algoritmo inválido, usando FIFO como padrão.\n\n");
            algoritmo = FIFO;
            break;
    }

    //
    loopSimulador(criarSimulador(tamanhoPagina, tamanhoMemoria, algoritmo, numFrames, totalProcessos, tamanhoProcessos, tempo_total_sistema));
}


int main() {
    int opcao;
    
    while(1) {
        printf("\nMenu Principal:\n");
        printf("1. Criar nova simulação\n");
        printf("2. Sair do programa\n");
        
        opcao = lerInteiro("Digite sua opção: ");
        
        switch(opcao) {
            case 1:
                printf("\nIniciando nova simulação...\n");
                novaSimulacao();
                break;
            case 2:
                printf("Encerrando o programa...\n");
                exit(0);
            default:
                printf("Opção inválida! Digite 1 ou 2.\n");
        }
    }
    return 0;
}