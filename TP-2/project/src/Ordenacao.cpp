#include "Ordenacao.h"

// Mergesort para IDs externos - O(nlogn) de tempo e O(n) de espaco auxiliar
static void mergeSort(int *dados, int n, int *aux)
{
    if (n <= 1)
        return;

    // Divide o vetor em duas metades
    int meio = n / 2;
    mergeSort(dados, meio, aux);
    mergeSort(dados + meio, n - meio, aux);

    // Merge das duas metades ordenadas em aux
    int i = 0, j = meio, k = 0;
    while (i < meio && j < n)
    {
        aux[k++] = (dados[i] <= dados[j]) ? dados[i++] : dados[j++];
    }

    // Copia o restante da metade esquerda
    while (i < meio)
        aux[k++] = dados[i++];

    // Copia o restante da metade direita
    while (j < n)
        aux[k++] = dados[j++];

    // Copia de volta para dados
    for (int x = 0; x < n; x++)
    {
        dados[x] = aux[x];
    }
}

// Interface publica do mergesort para IDs
void Ordenacao::mergeSortIds(int *dados, int n, int *aux)
{
    if (dados == 0 || aux == 0 || n <= 1)
        return;

    mergeSort(dados, n, aux);
}