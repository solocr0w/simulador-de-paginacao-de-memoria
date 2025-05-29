#include "processo.h"
#include <stdio.h>
#include <stdlib.h>


Processo* processo_criar(int pid, int tamanho, int tamanho_pagina){
    Processo *processo = malloc(sizeof(Processo));

    processo->pid = pid;
    processo->tamanho = tamanho;
    processo->num_paginas = (tamanho + tamanho_pagina - 1) / tamanho_pagina; // Arredonda para cima
    processo->tabela = tabela_paginas_criar(pid, processo->num_paginas);
    
    return processo;
}

//Extrai o número da página e o deslocamento de um endereço virtual
void processo_extrair_endereco(Processo *processo, int endereco_virtual, int *num_pagina, int *deslocamento, int tamanho_pagina){

    if (!processo || !num_pagina || !deslocamento) {
        fprintf(stderr, "Processo ou ponteiros inválidos.\n");
        return;
    }
    
    *num_pagina = endereco_virtual / tamanho_pagina; // Número da página
    *deslocamento = endereco_virtual % tamanho_pagina; // Deslocamento dentro da página
    
    if (*num_pagina >= processo->num_paginas) {
        fprintf(stderr, "Endereço virtual fora dos limites do processo.\n");
        *num_pagina = -1; // Indica erro
        *deslocamento = -1; // Indica erro
    }

}


