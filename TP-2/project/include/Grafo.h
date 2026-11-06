#ifndef GRAFO_H
#define GRAFO_H

#include "No.h"
// TAD Grafo - representa o grafo de temas e o grafo social de forma generica

enum ModoArmazenamento {
    LISTA_ADJ,
    MATRIZ_ADJ,
    MATRIZ_ESPARSA
};

// CelulaAdj - estrutura para representar uma celula de uma lista de adjacencia
struct CelulaAdj {
    int destino;
    CelulaAdj* prox;
};

class Grafo {
private:
    No* vertices;       // Lista de vertices do grafo, onde cada um e um No
    int numVertices;    // Qtd de vertices ativos no grafo
    int capVertices;    // Capacidade atual do vetor de vertices (para redimensionamento)
    int numArestas;     // Qtd de arestas armazenadas no grafo
    bool direcionado;   // true para o grafo social, false para o grafo de temas
    ModoArmazenamento modo;

    // Estruturas de armazenamento dos modos para as arestas
    CelulaAdj** listaSaida;     // Lista de saída (de u para v)
    CelulaAdj** listaEntrada;   // Lista de entrada (de v para u), usada apenas para o grafo social (direcionado)
    bool** matriz;              // Matriz booleana (true se existe aresta de u para v, false caso contrario)

    // Matriz esparsa guarda so os pares (linha, coluna) que existem
    // No grafo nao direcionado, a mesma aresta aparece nos dois sentidos
    int* esparsaLinhas;         
    int* esparsaColunas;
    int numParesEsparsos;
    int capParesEsparsos;

    void inicializar(int capacidadeInicial, ModoArmazenamento modoInicial);
    void redimensionar();

    // Metodos para o modo lista
    void alocarListas(int capacidade);
    void liberarListas();
    void liberarListaDeUmVertice(CelulaAdj* celula);

    // Metodos para o modo matriz
    void alocarMatriz(int capacidade);
    void liberarMatriz();

    // Metodos para o modo matriz esparsa
    void alocarMatrizEsparsa(int capacidadeInicial);
    void liberarMatrizEsparsa();
    void redimensionarMatrizEsparsa();

    // Insere par direto, sem regra de direcao e sem mexer em numArestas
    // Usado nas migracoes, quando os pares ja vem prontos
    void inserirParEsparsoDireto(int origem, int destino);
    bool removerParEsparso(int origem, int destino);
    bool existeParEsparso(int origem, int destino) const;

    // Metodos para manpular as listas encadeadas
    void inserirCelula(CelulaAdj*& inicio, int destino);
    bool removerCelula(CelulaAdj*& inicio, int destino);
    bool existeNaLista(CelulaAdj* inicio, int destino) const;
    int copiarListaParaBuffer(CelulaAdj* inicio, int* buffer, int capacidade) const;

    // Metodos de migracao entre modos
    void migrarListaParaMatriz();
    void migrarMatrizParaLista();
    void migrarListaParaMatrizEsparsa();
    void migrarMatrizEsparsaParaLista();

    Grafo(const Grafo&);
    Grafo& operator=(const Grafo&);

public:
    //Construtor e destrutor do grafo
    // capacidadeInicial = 8 por decisao de projeto, meio termo entre muitos redimensionamentos iniciais e desperdicio de memoria
    Grafo(bool direcionado, ModoArmazenamento modoInicial, int capacidadeInicial = 8);
    ~Grafo();

    int adicionarVertice(int idExterno, TipoNo tipo);

    //Manipulacao e consulta de arestas
    void inserirAresta(int origem, int destino);
    void removerAresta(int origem, int destino);
    bool existeAresta(int origem, int destino) const;

    // Preenche temp com indices internos dos vizinhos/predecessores e retorna a quantidade preenchida
    int obterVizinhos(int indice, int* temp, int capacidade) const;         // Arestas de saida
    int obterPredecessores(int indice, int* temp, int capacidade) const;    // Arestas de entrada

    void migrar(ModoArmazenamento novoModo);

    // Getters padrao
    No getVertice(int indice) const;
    int getNumVertices() const;
    int getNumArestas() const;
    bool isDirecionado() const;
    ModoArmazenamento getModo() const;
};

#endif
