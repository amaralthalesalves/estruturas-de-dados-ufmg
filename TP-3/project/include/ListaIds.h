#ifndef LISTA_IDS_H
#define LISTA_IDS_H

// Lista simples de ids; nos indices, a ordem de insercao ja acompanha a ordem dos cadastros
class ListaIds {
private:
    int* ids;
    int quantidade;
    int capacidade;

    void expandir();

    ListaIds(const ListaIds& outra);
    ListaIds& operator=(const ListaIds& outra);

public:
    ListaIds();
    ~ListaIds();

    void inicializar(int capacidadeInicial = 4);
    void destruir();
    void inserir(int id);

    int getId(int posicao) const;
    int getQuantidade() const;
    int getCapacidade() const;
};

#endif
