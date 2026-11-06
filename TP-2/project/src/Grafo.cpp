#include "Grafo.h"

// Cria um grafo direcionado ou nao, no modo de armazenamento especificado
Grafo::Grafo(bool direcionado, ModoArmazenamento modoInicial, int capacidadeInicial)
    : vertices(0),
      numVertices(0),
      capVertices(0),
      numArestas(0),
      direcionado(direcionado),
      modo(modoInicial),
      listaSaida(0),
      listaEntrada(0),
      matriz(0),
      esparsaLinhas(0),
      esparsaColunas(0),
      numParesEsparsos(0),
      capParesEsparsos(0)
{
    inicializar(capacidadeInicial, modoInicial);
}

Grafo::~Grafo()
{
    if (modo == LISTA_ADJ)
    {
        liberarListas();
    }
    else if (modo == MATRIZ_ADJ)
    {
        liberarMatriz();
    }
    else
    {
        liberarMatrizEsparsa();
    }

    delete[] vertices;
}

void Grafo::inicializar(int capacidadeInicial, ModoArmazenamento modoInicial)
{
    if (capacidadeInicial < 1)
    {
        capacidadeInicial = 8;
    }

    capVertices = capacidadeInicial;
    numVertices = 0;
    numArestas = 0;
    modo = modoInicial;

    vertices = new No[capVertices];

    if (modo == LISTA_ADJ)
    {
        alocarListas(capVertices);
    }
    else if (modo == MATRIZ_ADJ)
    {
        alocarMatriz(capVertices);
    }
    else
    {
        alocarMatrizEsparsa(capVertices);
    }
}
// Aloca as listas de entrada e saida, inicializando os ponteiros com 0
void Grafo::alocarListas(int capacidade)
{
    listaSaida = new CelulaAdj *[capacidade];
    for (int i = 0; i < capacidade; i++)
    {
        listaSaida[i] = 0;
    }
    // Verifica se o grafo e direcionado ou nao Se for, aloca a lista de entrada, que representa as arestas
    // que chegam no vertice Utilizada para consultar predecessores
    if (direcionado)
    {
        listaEntrada = new CelulaAdj *[capacidade];
        for (int i = 0; i < capacidade; i++)
        {
            listaEntrada[i] = 0;
        }
    }
    else
    {
        listaEntrada = 0;
    }
}

void Grafo::liberarListas()
{
    if (listaSaida != 0)
    {
        for (int i = 0; i < capVertices; i++)
        {
            liberarListaDeUmVertice(listaSaida[i]);
        }
        delete[] listaSaida;
        listaSaida = 0;
    }

    if (listaEntrada != 0)
    {
        for (int i = 0; i < capVertices; i++)
        {
            liberarListaDeUmVertice(listaEntrada[i]);
        }
        delete[] listaEntrada;
        listaEntrada = 0;
    }
}

void Grafo::liberarListaDeUmVertice(CelulaAdj *celula)
{
    while (celula != 0)
    {
        CelulaAdj *proxima = celula->prox;
        delete celula;
        celula = proxima;
    }
}

void Grafo::alocarMatriz(int capacidade)
{
    matriz = new bool *[capacidade];
    // Inicializa a matriz com false, indicando que nao existem arestas
    for (int i = 0; i < capacidade; i++)
    {
        matriz[i] = new bool[capacidade];
        for (int j = 0; j < capacidade; j++)
        {
            matriz[i][j] = false;
        }
    }
}

void Grafo::liberarMatriz()
{
    if (matriz == 0)
    {
        return;
    }

    for (int i = 0; i < capVertices; i++)
    {
        delete[] matriz[i];
    }
    delete[] matriz;
    matriz = 0;
}

// ######### PONTO EXTRA - MATRIZ ESPARSA #########
void Grafo::alocarMatrizEsparsa(int capacidadeInicial)
{
    if (capacidadeInicial < 1)
    {
        capacidadeInicial = 8;
    }

    // Capacidade aqui e de pares, nao de vertices
    capParesEsparsos = capacidadeInicial;
    numParesEsparsos = 0;
    esparsaLinhas = new int[capParesEsparsos];
    esparsaColunas = new int[capParesEsparsos];
}

void Grafo::liberarMatrizEsparsa()
{
    delete[] esparsaLinhas;
    delete[] esparsaColunas;
    esparsaLinhas = 0;
    esparsaColunas = 0;
    numParesEsparsos = 0;
    capParesEsparsos = 0;
}

void Grafo::redimensionarMatrizEsparsa()
{
    // Dobra a capacidade dos pares esparsos
    int novaCapacidade = capParesEsparsos * 2;
    if (novaCapacidade < 1)
    {
        novaCapacidade = 8;
    }

    int *novasLinhas = new int[novaCapacidade];
    int *novasColunas = new int[novaCapacidade];

    for (int i = 0; i < numParesEsparsos; i++)
    {
        novasLinhas[i] = esparsaLinhas[i];
        novasColunas[i] = esparsaColunas[i];
    }

    delete[] esparsaLinhas;
    delete[] esparsaColunas;

    esparsaLinhas = novasLinhas;
    esparsaColunas = novasColunas;
    capParesEsparsos = novaCapacidade;
}

void Grafo::inserirParEsparsoDireto(int origem, int destino)
{
    // Nao verifica duplicata nem mexe em numArestas
    // Assim a migracao copia os pares do jeito que eles ja estao
    if (numParesEsparsos == capParesEsparsos)
    {
        redimensionarMatrizEsparsa();
    }

    esparsaLinhas[numParesEsparsos] = origem;
    esparsaColunas[numParesEsparsos] = destino;
    numParesEsparsos++;
}

bool Grafo::removerParEsparso(int origem, int destino)
{
    for (int i = 0; i < numParesEsparsos; i++)
    {
        if (esparsaLinhas[i] == origem && esparsaColunas[i] == destino)
        {
            // A ordem interna nao importa, entao troca com o ultimo
            int ultimo = numParesEsparsos - 1;
            esparsaLinhas[i] = esparsaLinhas[ultimo];
            esparsaColunas[i] = esparsaColunas[ultimo];
            numParesEsparsos--;
            return true;
        }
    }

    return false;
}

bool Grafo::existeParEsparso(int origem, int destino) const
{
    for (int i = 0; i < numParesEsparsos; i++)
    {
        if (esparsaLinhas[i] == origem && esparsaColunas[i] == destino)
        {
            return true;
        }
    }

    return false;
}

// Converte listas para matriz esparsa copiando os pares que ja existem nas listas
void Grafo::migrarListaParaMatrizEsparsa()
{
    alocarMatrizEsparsa(numArestas > 0 ? numArestas : capVertices);

    for (int i = 0; i < numVertices; i++)
    {
        CelulaAdj *atual = listaSaida[i];
        while (atual != 0)
        {
            inserirParEsparsoDireto(i, atual->destino);
            atual = atual->prox;
        }
    }

    liberarListas();
    modo = MATRIZ_ESPARSA;
}

// Converte matriz esparsa para listas mantendo exatamente os pares guardados
void Grafo::migrarMatrizEsparsaParaLista()
{
    alocarListas(capVertices);

    for (int i = 0; i < numParesEsparsos; i++)
    {
        inserirCelula(listaSaida[esparsaLinhas[i]], esparsaColunas[i]);

        if (direcionado)
        {
            inserirCelula(listaEntrada[esparsaColunas[i]], esparsaLinhas[i]);
        }
    }

    liberarMatrizEsparsa();
    modo = LISTA_ADJ;
}

// ##################################################

// Insere a celula no inicio da lista para manter a operacao O(1)
// A ordenacao crescente nas saidas e tratada depois
void Grafo::inserirCelula(CelulaAdj *&inicio, int destino)
{
    CelulaAdj *nova = new CelulaAdj;
    nova->destino = destino;
    nova->prox = inicio;
    inicio = nova;
}

bool Grafo::removerCelula(CelulaAdj *&inicio, int destino)
{
    CelulaAdj *atual = inicio;
    CelulaAdj *anterior = 0;

    while (atual != 0)
    {
        if (atual->destino == destino)
        {
            if (anterior == 0)
            {
                inicio = atual->prox;
            }
            else
            {
                anterior->prox = atual->prox;
            }
            delete atual;
            return true;
        }

        anterior = atual;
        atual = atual->prox;
    }

    return false;
}

bool Grafo::existeNaLista(CelulaAdj *inicio, int destino) const
{
    CelulaAdj *atual = inicio;
    while (atual != 0)
    {
        if (atual->destino == destino)
        {
            return true;
        }
        atual = atual->prox;
    }
    return false;
}

int Grafo::copiarListaParaBuffer(CelulaAdj *inicio, int *buffer, int capacidade) const
{
    int quantidade = 0;
    CelulaAdj *atual = inicio;

    while (atual != 0 && quantidade < capacidade)
    {
        buffer[quantidade] = atual->destino;
        quantidade++;
        atual = atual->prox;
    }

    return quantidade;
}

// Dobra a capacidade do grafo quando o array de vertices fica cheio
// As arestas existentes precisam ser preservadas em qualquer representacao
void Grafo::redimensionar()
{
    int capacidadeAntiga = capVertices;
    int novaCapacidade = capVertices * 2;

    No *novosVertices = new No[novaCapacidade];
    for (int i = 0; i < numVertices; i++)
    {
        novosVertices[i] = vertices[i];
    }

    delete[] vertices;
    vertices = novosVertices;

    if (modo == LISTA_ADJ)
    {
        CelulaAdj **novaListaSaida = new CelulaAdj *[novaCapacidade];
        for (int i = 0; i < novaCapacidade; i++)
        {
            novaListaSaida[i] = 0;
        }
        for (int i = 0; i < numVertices; i++)
        {
            novaListaSaida[i] = listaSaida[i];
        }
        delete[] listaSaida;
        listaSaida = novaListaSaida;

        if (direcionado)
        {
            CelulaAdj **novaListaEntrada = new CelulaAdj *[novaCapacidade];
            for (int i = 0; i < novaCapacidade; i++)
            {
                novaListaEntrada[i] = 0;
            }
            for (int i = 0; i < numVertices; i++)
            {
                novaListaEntrada[i] = listaEntrada[i];
            }
            delete[] listaEntrada;
            listaEntrada = novaListaEntrada;
        }
    }
    else if (modo == MATRIZ_ADJ)
    {
        // Copia a matriz antiga para a nova
        // As posições novas permanecem false, ja que nao tem arestas nelas
        bool **novaMatriz = new bool *[novaCapacidade];
        for (int i = 0; i < novaCapacidade; i++)
        {
            novaMatriz[i] = new bool[novaCapacidade];
            for (int j = 0; j < novaCapacidade; j++)
            {
                novaMatriz[i][j] = false;
            }
        }

        for (int i = 0; i < capacidadeAntiga; i++)
        {
            for (int j = 0; j < capacidadeAntiga; j++)
            {
                novaMatriz[i][j] = matriz[i][j];
            }
        }

        liberarMatriz();
        matriz = novaMatriz;
    }

    capVertices = novaCapacidade;
}

int Grafo::adicionarVertice(int idExterno, TipoNo tipo)
{
    if (numVertices == capVertices)
    {
        redimensionar();
    }

    vertices[numVertices].idExterno = idExterno;
    vertices[numVertices].tipo = tipo;

    numVertices++;
    return numVertices - 1;
}

// Insere uma aresta entre indices internos
// Lista: O(1), pois insere no inicio Matriz: O(1) Esparsa: O(1) amortizado
void Grafo::inserirAresta(int origem, int destino)
{
    if (modo == LISTA_ADJ)
    {
        inserirCelula(listaSaida[origem], destino);

        if (direcionado)
        {
            // Em grafo direcionado, deve manter a lista de entrada atualizada
            inserirCelula(listaEntrada[destino], origem);
        }
        else
        {
            inserirCelula(listaSaida[destino], origem);
        }
    }
    else if (modo == MATRIZ_ADJ)
    {
        matriz[origem][destino] = true;

        if (!direcionado)
        {
            matriz[destino][origem] = true;
        }
    }
    else
    {
        inserirParEsparsoDireto(origem, destino);

        if (!direcionado)
        {
            inserirParEsparsoDireto(destino, origem);
        }
    }

    numArestas++;
}
// Remove uma aresta entre indices internos
// Lista: O(grau), pois precisa buscar a celula Matriz: O(1) Esparsa: O(E)
void Grafo::removerAresta(int origem, int destino)
{
    if (modo == LISTA_ADJ)
    {
        removerCelula(listaSaida[origem], destino);

        if (direcionado)
        {
            // Em grafo direcionado, manter lista de entrada atualizada
            removerCelula(listaEntrada[destino], origem);
        }
        else
        {
            removerCelula(listaSaida[destino], origem);
        }
    }
    else if (modo == MATRIZ_ADJ)
    {
        matriz[origem][destino] = false;

        if (!direcionado)
        {
            matriz[destino][origem] = false;
        }
    }
    else
    {
        removerParEsparso(origem, destino);

        if (!direcionado)
        {
            removerParEsparso(destino, origem);
        }
    }

    numArestas--;
}
// Consulta se existe aresta entre dois indices internos
// Lista: O(grau_saida(origem)) Matriz: O(1) Esparsa: O(E)
bool Grafo::existeAresta(int origem, int destino) const
{
    if (modo == LISTA_ADJ)
    {
        return existeNaLista(listaSaida[origem], destino);
    }

    if (modo == MATRIZ_ESPARSA)
    {
        return existeParEsparso(origem, destino);
    }

    return matriz[origem][destino];
}
// Preenche o buffer com os indices internos dos vizinhos de saida
// Lista: O(grau_saida(indice)) Matriz: O(V) Esparsa: O(E)
int Grafo::obterVizinhos(int indice, int *buffer, int capacidade) const
{
    if (buffer == 0 || capacidade <= 0)
    {
        return 0;
    }

    if (modo == LISTA_ADJ)
    {
        return copiarListaParaBuffer(listaSaida[indice], buffer, capacidade);
    }

    if (modo == MATRIZ_ESPARSA)
    {
        int quantidade = 0;
        for (int i = 0; i < numParesEsparsos && quantidade < capacidade; i++)
        {
            if (esparsaLinhas[i] == indice)
            {
                buffer[quantidade] = esparsaColunas[i];
                quantidade++;
            }
        }
        return quantidade;
    }

    int quantidade = 0;
    for (int j = 0; j < numVertices && quantidade < capacidade; j++)
    {
        if (matriz[indice][j])
        {
            buffer[quantidade] = j;
            quantidade++;
        }
    }

    return quantidade;
}

// Preenche o buffer com os indices internos dos predecessores
// Lista direcionada: O(grau_entrada(indice)) Matriz: O(V) Esparsa: O(E)
int Grafo::obterPredecessores(int indice, int *buffer, int capacidade) const
{
    if (buffer == 0 || capacidade <= 0)
    {
        return 0;
    }
    // Em grafo nao direcionado, os vizinhos tambem sao predecessores
    if (!direcionado)
    {
        return obterVizinhos(indice, buffer, capacidade);
    }

    if (modo == LISTA_ADJ)
    {
        return copiarListaParaBuffer(listaEntrada[indice], buffer, capacidade);
    }

    if (modo == MATRIZ_ESPARSA)
    {
        int quantidade = 0;
        for (int i = 0; i < numParesEsparsos && quantidade < capacidade; i++)
        {
            if (esparsaColunas[i] == indice)
            {
                buffer[quantidade] = esparsaLinhas[i];
                quantidade++;
            }
        }
        return quantidade;
    }

    int quantidade = 0;
    for (int i = 0; i < numVertices && quantidade < capacidade; i++)
    {
        if (matriz[i][indice])
        {
            buffer[quantidade] = i;
            quantidade++;
        }
    }

    return quantidade;
}
// Troca o modo de armazenamento ativo do grafo
void Grafo::migrar(ModoArmazenamento novoModo)
{
    if (novoModo == modo)
    {
        return;
    }

    if (modo == LISTA_ADJ && novoModo == MATRIZ_ADJ)
    {
        migrarListaParaMatriz();
    }
    else if (modo == MATRIZ_ADJ && novoModo == LISTA_ADJ)
    {
        migrarMatrizParaLista();
    }
    else if (modo == LISTA_ADJ && novoModo == MATRIZ_ESPARSA)
    {
        migrarListaParaMatrizEsparsa();
    }
    else if (modo == MATRIZ_ESPARSA && novoModo == LISTA_ADJ)
    {
        migrarMatrizEsparsaParaLista();
    }
    else if (modo == MATRIZ_ADJ && novoModo == MATRIZ_ESPARSA)
    {
        migrarMatrizParaLista();
        migrarListaParaMatrizEsparsa();
    }
    else if (modo == MATRIZ_ESPARSA && novoModo == MATRIZ_ADJ)
    {
        migrarMatrizEsparsaParaLista();
        migrarListaParaMatriz();
    }
}
// Converte listas de adjacencia para matriz
// Complexidade: O(V^2 + E), pois inicializa a matriz inteira e copia as arestas
void Grafo::migrarListaParaMatriz()
{
    bool **novaMatriz = new bool *[capVertices];

    // A nova matriz comeca vazia, sem arestas As arestas existentes serao copiadas depois
    for (int i = 0; i < capVertices; i++)
    {
        novaMatriz[i] = new bool[capVertices];
        for (int j = 0; j < capVertices; j++)
        {
            novaMatriz[i][j] = false;
        }
    }

    // Cada celula na lista de saida representa uma aresta de i para destino Copia essas arestas para a nova matriz
    for (int i = 0; i < numVertices; i++)
    {
        CelulaAdj *atual = listaSaida[i];
        while (atual != 0)
        {
            novaMatriz[i][atual->destino] = true;
            atual = atual->prox;
        }
    }

    liberarListas();
    matriz = novaMatriz;
    modo = MATRIZ_ADJ;
}

// Converte matriz para listas de adjacencia
// Complexidade: O(V^2), pois precisa varrer toda a matriz
void Grafo::migrarMatrizParaLista()
{
    alocarListas(capVertices);
    // Reconstroi as listas a partir da matriz Se matriz[i][j] == true, existe uma aresta de i para j
    // Em grafo nao direcionado, a matriz é simetrica, entao a aresta de j pra i ja sera copiada quando chegar na linha j
    for (int i = 0; i < numVertices; i++)
    {
        for (int j = 0; j < numVertices; j++)
        {
            if (matriz[i][j])
            {
                inserirCelula(listaSaida[i], j);

                if (direcionado)
                {
                    inserirCelula(listaEntrada[j], i);
                }
            }
        }
    }
    liberarMatriz();
    modo = LISTA_ADJ;
}

// Getters e setters basicos
No Grafo::getVertice(int indice) const
{
    return vertices[indice];
}

int Grafo::getNumVertices() const
{
    return numVertices;
}

int Grafo::getNumArestas() const
{
    return numArestas;
}

bool Grafo::isDirecionado() const
{
    return direcionado;
}

ModoArmazenamento Grafo::getModo() const
{
    return modo;
}
