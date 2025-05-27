#ifndef PAGINA_H
#define PAGINA_H

#include <stdbool.h>

typedef struct {
    bool presente;         // 1 se está na memória física, 0 caso contrário
    int frame;             // Número do frame na memória física (-1 se não presente)
    bool modificada;       // Bit "M" (página foi modificada)
    bool referenciada;     // Bit "R" (página foi acessada recentemente)
    int tempo_carga;       // Instante em que a página foi carregada (para FIFO)
    int ultimo_acesso;     // Instante do último acesso (para LRU)
} Pagina;

#endif