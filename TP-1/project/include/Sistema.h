#ifndef SISTEMA_H
#define SISTEMA_H

#include "Acao.h"
#include "Cliente.h"
#include "Ranqueamento.h"

struct Sistema {
    int w, N, u, numMetricas; // w - janela; N - numero de acoes; u - numero de clientes; numMetricas - numero de metricas
    int metricaIds[4];        // indice global de cada metrica ativa (0=RET,1=AVGRET,2=STAB,3=CONS) pra funcionar com o switch do Acao.cpp

    Acao* acoes;        //tamanho N
    Cliente* clientes;  //tamanho u

    //Estrategia imediata - ranking pre-calculado e mantido a cada atualizacao de preco
    int** rankingMetrica;   //[numMetricas][N]: IDs de acao ordenados por metrica (decrescente)

    //Espaco de trabalho reutilizavel (alocado uma vez, liberado em destruir)
    int* posicaoNaMetrica;          //[numMetricas*N] - para cada metrica k, as primeiras N posicoes sao usadas para
                                    //armazenar a posicao de cada acao nessa metrica. Acesso [k*N + acao], retorna
                                    //a posicao da acao no ranking da metrica k
    AcaoMetrica* ordMetrica;        //tamanho N
    AcaoMetrica* ordMetricaAux;     //tamanho N (auxiliar do mergesort)
    AcaoPontuacao* ordPontuacao;    //tamanho N
    AcaoPontuacao* ordPontuacaoAux; //tamanho N (auxiliar do mergesort)

    void inicializar(int w, int N, int u, int numMetricas, int* ids);
    void destruir();
    void tratarCotacao(int acao, double preco); //comando P
    void tratarCompra(int cliente, int acao);   //comando B
    void tratarVenda(int cliente, int acao);    //comando V
    void tratarConsulta(int idConsulta, int cliente, int qtdAcoesDestaque, int qtdMetricas, int* queryMetricaIds, double* pesos); //Comando Q
};

#endif
