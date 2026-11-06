#include "Ranqueamento.h"

//Comparador de acoes por metrica - retorna true se a1 deve vir ANTES de a2 na ordenacao
//Criterio - decrescente por valor, desempate crescente por ID da acao
static bool metricaMenor(const AcaoMetrica& a1, const AcaoMetrica& a2) {
    if (a1.valor != a2.valor) return a1.valor > a2.valor;
    else return a1.acao < a2.acao;
}

//Comparador de acoes por pontuacao - retorna true se a1 deve vir ANTES de b1 na ordenacao
//Criterio - decrescente por pontuacao, desempate crescente por ID da acao
static bool pontuacaoMenor(const AcaoPontuacao& a1, const AcaoPontuacao& a2) {
    if (a1.pontuacao != a2.pontuacao) return a1.pontuacao > a2.pontuacao;
    else return a1.acao < a2.acao;
}

//Mergesort para AcaoMetrica - O(nlogn) de tempo e O(n) de espaco
void mergesortMetrica(AcaoMetrica* arr, int n, AcaoMetrica* aux) {
    if (n <= 1) return;

    int meio = n / 2;
    mergesortMetrica(arr, meio, aux);
    mergesortMetrica(arr + meio, n - meio, aux);

    //Merge das duas metades em aux
    int i = 0, j = meio, k = 0;
    while (i < meio && j < n)
        aux[k++] = metricaMenor(arr[i], arr[j]) ? arr[i++] : arr[j++];
    while (i < meio) aux[k++] = arr[i++];
    while (j < n)    aux[k++] = arr[j++];

    //Copia de volta para arr
    for (int x = 0; x < n; x++) arr[x] = aux[x];
}

//Mergesort para AcaoPontuacao - O(nlogn) de tempo e O(n) de espaco
void mergesortPontuacao(AcaoPontuacao* arr, int n, AcaoPontuacao* aux) {
    if (n <= 1) return;

    int meio = n / 2;
    mergesortPontuacao(arr, meio, aux);
    mergesortPontuacao(arr + meio, n - meio, aux);

    //Merge das duas metades em aux
    int i = 0, j = meio, k = 0;
    while (i < meio && j < n)
        aux[k++] = pontuacaoMenor(arr[i], arr[j]) ? arr[i++] : arr[j++];
    while (i < meio) aux[k++] = arr[i++];
    while (j < n)    aux[k++] = arr[j++];

    //Copia de volta para arr
    for (int x = 0; x < n; x++) arr[x] = aux[x];
}
