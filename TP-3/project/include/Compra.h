#ifndef COMPRA_H
#define COMPRA_H

#include "Tipos.h"

// Compra ja aceita pelo sistema; compras invalidas nem chegam a ser armazenadas
class Compra {
private:
    int id;
    int timestamp;
    int idUsuario;
    ItemEstoque* itens;
    int numItens;
    int capItens;

    void copiarDe(const Compra& outra);

    Compra(const Compra& outra);

public:
    Compra();
    Compra(int id, int timestamp, int idUsuario, const ItemEstoque* itens, int numItens);
    ~Compra();

    Compra& operator=(const Compra& outra);

    void inicializar(int id, int timestamp, int idUsuario, const ItemEstoque* itens, int numItens);
    void destruir();

    int getId() const;
    int getTimestamp() const;
    int getIdUsuario() const;
    const ItemEstoque* getItens() const;
    int getNumItens() const;
};

#endif
