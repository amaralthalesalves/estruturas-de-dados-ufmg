#ifndef INDICES_H
#define INDICES_H

#include "IndiceInvertido.h"

// Agrupadores simples dos indices que o Sistema precisa manter para cada entidade
struct IndicesUsuarios {
    IndiceInvertido nome;
    IndiceInvertido idade;
    IndiceInvertido cidade;
    IndiceInvertido estado;
    IndiceInvertido nacionalidade;

    void inicializar();
    void destruir();
};

struct IndicesProdutos {
    IndiceInvertido nome;
    IndiceInvertido categoria;
    IndiceInvertido marca;
    IndiceInvertido condicao;

    void inicializar();
    void destruir();
};

struct IndicesCompras {
    IndiceInvertido timestamp;
    IndiceInvertido usuario;
    IndiceInvertido produto;

    void inicializar();
    void destruir();
};

struct IndicesReposicoes {
    IndiceInvertido timestamp;
    IndiceInvertido produto;

    void inicializar();
    void destruir();
};

#endif
