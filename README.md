# Simulador de Paginação de Memória

Este projeto tem como objetivo simular o processo de paginação de memória em sistemas operacionais, incluindo a tradução de endereços virtuais em físicos, a alocação de páginas e a aplicação de diferentes algoritmos de substituição de páginas. Através de uma arquitetura modular, o simulador busca reproduzir de forma didática o funcionamento de um gerenciador de memória.

Alunos: Bruno Germanetti Ramalho, Camila Nunes Carniel, Gabriel Erick Mendes, Mateus Teles Magalhães, Miguel Coratolo Simões Piñeiro.

## 📚 Descrição da Implementação

A arquitetura do projeto é dividida em múltiplos módulos:

- **Simulador** (`simulador.c/.h`): núcleo principal, responsável por coordenar a execução, gerenciar os ciclos de tempo, criar processos aleatórios, simular acessos à memória e coletar estatísticas como page faults.
- **Memória Física** (`memoria.c/.h`): representada por um vetor de frames, lida com a alocação e desalocação de páginas na RAM.
- **Processo** (`processo.c/.h`): representa processos ativos no sistema, contendo dados como PID, número de páginas e uma tabela de páginas.
- **Tabela de Páginas** (`tabela-pagina.c/.h`): estrutura usada para traduzir endereços virtuais em físicos, além de armazenar informações como frame alocado, bit de presença, modificação e último acesso.
- **Algoritmos de Substituição** (`algoritmosSubstituicao.c/.h`): implementação dos algoritmos FIFO, LRU, Clock e Random, utilizados para gerenciar quais páginas devem ser substituídas em casos de page fault.

## ⚙️ Principais Funções e Algoritmos

- `algoritmo_FIFO`, `algoritmo_LRU`, `algoritmo_Clock`, `algoritmo_RANDOM`: algoritmos de substituição de página.
- `memoria_alocar_frame`, `memoria_alocar_frame_livre`, `removerFrame`: gerenciamento de alocação na RAM.
- `loopSimulador`: simula os ciclos de tempo e os acessos à memória, aplicando os algoritmos selecionados.
- `tabela_paginas_criar`, `tabela_paginas_atualizar_presente`, `tabela_paginas_remover_pagina`: controle da tabela de páginas.

## 🧠 Decisões de Projeto

- **Modularização Clara**: o código é dividido em arquivos/módulos bem definidos, cada um responsável por uma parte do sistema.
- **Flexibilidade de Algoritmos**: o simulador permite alternar facilmente entre os algoritmos de substituição.


