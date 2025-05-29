#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"
#include "memoria.h"
#include "processo.h"
#include "tabela-pagina.h"
#include "pagina.h"


void removerPagina(Simulador *sim, int frame_removido) {
    for (int i = 0; i < sim->num_processos; i++) {
        Processo *proc = sim->processos[i];
        for (int j = 0; j < proc->num_paginas; j++) {
            Pagina *pagina = &proc->tabela->paginas[j];
            if (pagina->presente && pagina->frame == frame_removido) {
                pagina->presente = false;
                pagina->frame = -1;
                return;
            }
        }
    }
}

