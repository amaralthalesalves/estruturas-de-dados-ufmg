#include "Indices.h"

void IndicesUsuarios::inicializar() {
    nome.inicializar();
    idade.inicializar();
    cidade.inicializar();
    estado.inicializar();
    nacionalidade.inicializar();
}

void IndicesUsuarios::destruir() {
    nome.destruir();
    idade.destruir();
    cidade.destruir();
    estado.destruir();
    nacionalidade.destruir();
}

void IndicesProdutos::inicializar() {
    nome.inicializar();
    categoria.inicializar();
    marca.inicializar();
    condicao.inicializar();
}

void IndicesProdutos::destruir() {
    nome.destruir();
    categoria.destruir();
    marca.destruir();
    condicao.destruir();
}

void IndicesCompras::inicializar() {
    timestamp.inicializar();
    usuario.inicializar();
    produto.inicializar();
}

void IndicesCompras::destruir() {
    timestamp.destruir();
    usuario.destruir();
    produto.destruir();
}

void IndicesReposicoes::inicializar() {
    timestamp.inicializar();
    produto.inicializar();
}

void IndicesReposicoes::destruir() {
    timestamp.destruir();
    produto.destruir();
}
