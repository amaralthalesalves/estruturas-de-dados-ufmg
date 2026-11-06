#ifndef BUFFERS_CONSULTA_H
#define BUFFERS_CONSULTA_H

#include "Recomendador.h"
// TAD BuffersConsulta - estrutura para armazenar buffers reutilizaveis nas consultas, evitando alocacoes repetidas e melhorando a performance

struct BuffersConsulta
{
    int *indicesInternos; 
    int *idsExternos;
    int *auxOrdenacao;
    // Buffers usados no comando P
    int *filaBuscaSocial;
    int *distanciasSociais;
    int *temasMarcados;
    Recomendacao *recomendacoes;
    Recomendacao *auxRecomendacoes;
    int capacidade;

    BuffersConsulta()
        : indicesInternos(0),
          idsExternos(0),
          auxOrdenacao(0),
          filaBuscaSocial(0),
          distanciasSociais(0),
          temasMarcados(0),
          recomendacoes(0),
          auxRecomendacoes(0),
          capacidade(0) {}

    ~BuffersConsulta()
    {
        delete[] indicesInternos;
        delete[] idsExternos;
        delete[] auxOrdenacao;
        delete[] filaBuscaSocial;
        delete[] distanciasSociais;
        delete[] temasMarcados;
        delete[] recomendacoes;
        delete[] auxRecomendacoes;
    }
    // Realoca buffers de acordo com a capacidade necessári
    void garantirCapacidade(int novaCapacidade)
    {
        if (novaCapacidade < 1)
        {
            novaCapacidade = 1;
        }

        if (novaCapacidade <= capacidade)
        {
            return;
        }

        delete[] indicesInternos;
        delete[] idsExternos;
        delete[] auxOrdenacao;
        delete[] filaBuscaSocial;
        delete[] distanciasSociais;
        delete[] temasMarcados;
        delete[] recomendacoes;
        delete[] auxRecomendacoes;

        indicesInternos = new int[novaCapacidade];
        idsExternos = new int[novaCapacidade];
        auxOrdenacao = new int[novaCapacidade];
        filaBuscaSocial = new int[novaCapacidade];
        distanciasSociais = new int[novaCapacidade];
        temasMarcados = new int[novaCapacidade];
        recomendacoes = new Recomendacao[novaCapacidade];
        auxRecomendacoes = new Recomendacao[novaCapacidade];
        capacidade = novaCapacidade;
    }
};

#endif // BUFFERS_CONSULTA_H
