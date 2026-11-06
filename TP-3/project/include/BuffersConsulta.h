#ifndef BUFFERS_CONSULTA_H
#define BUFFERS_CONSULTA_H

#include "Tipos.h"

// Memoria temporaria reaproveitada pelo main nas consultas e na leitura de itens
struct BuffersConsulta {
    int* idsResultado;
    int* idsAux;
    int* idsFiltro;
    ItemEstoque* itensTemporarios;

    int capacidadeIds;
    int capacidadeItens;

    BuffersConsulta();
    ~BuffersConsulta();

    void garantirCapacidadeIds(int novaCapacidade);
    void garantirCapacidadeItens(int novaCapacidade);
    void destruir();
};

#endif
