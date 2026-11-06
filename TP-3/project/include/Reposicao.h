#ifndef REPOSICAO_H
#define REPOSICAO_H

#include "Tipos.h"

// Reposicao sempre e valida e registra entrada de estoque no historico
class Reposicao {
private:
    int id;
    int timestamp;
    ItemEstoque* itens;
    int numItens;
    int capItens;

    void copiarDe(const Reposicao& outra);

    Reposicao(const Reposicao& outra);

public:
    Reposicao();
    Reposicao(int id, int timestamp, const ItemEstoque* itens, int numItens);
    ~Reposicao();

    Reposicao& operator=(const Reposicao& outra);

    void inicializar(int id, int timestamp, const ItemEstoque* itens, int numItens);
    void destruir();

    int getId() const;
    int getTimestamp() const;
    const ItemEstoque* getItens() const;
    int getNumItens() const;
};

#endif
