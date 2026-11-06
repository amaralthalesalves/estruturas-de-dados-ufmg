#ifndef RANQUEAMENTO_H
#define RANQUEAMENTO_H

//Struct auxiliar para ordenar acoes por valor de metrica
//Ajuda a manter o acesso as acoes O(1), ja que nao reordena o array original, so as referencias
struct AcaoMetrica {
    int acao;
    double valor;
};

//Struct auxiliar para armazenar as acoes e suas pontuacoes das metricas utilizadas
struct AcaoPontuacao {
    int acao;
    double pontuacao;
};

//Mergesort para AcaoMetrica - decrescente por valor, desempate crescente por acao
void mergesortMetrica(AcaoMetrica* arr, int n, AcaoMetrica* tmp);

//Mergesort para AcaoPontuacao - decrescente por pontuacao, desempate crescente por acao
void mergesortPontuacao(AcaoPontuacao* arr, int n, AcaoPontuacao* tmp);

#endif
