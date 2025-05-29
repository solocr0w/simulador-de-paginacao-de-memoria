#ifndef PAGINA_H
#define PAGINA_H

#include <stdbool.h>

typedef struct {
    int frame;             // Número do frame na memória física (-1 se não presente)
    bool presente;         // 1 se está na memória física, 0 caso contrário
    bool modificada;       // Bit "M" (página foi modificada)
    bool referenciada;     // Bit "R" (página foi acessada recentemente)
    int tempo_carga;       // Instante em que a página foi carregada na memoria fisica (para FIFO)
    int ultimo_acesso;     // Instante do último acesso (para LRU)
} Pagina;

#endif