#ifndef INDICE_INVERTIDO_H
#define INDICE_INVERTIDO_H

#include "ListaIds.h"

#include <string>

// Cada entrada guarda um valor de atributo e os ids que possuem esse valor
struct EntradaIndice {
    std::string chave;
    ListaIds ids;
    bool ocupado;
};

// Indice invertido implementado com hash; o nome do TAD segue o conceito do enunciado
class IndiceInvertido {
private:
    EntradaIndice* tabela;
    int capacidade;
    int quantidade;

    int calcularHash(const std::string& chave) const;
    int buscarPosicao(const std::string& chave) const;
    int buscarPosicaoInsercao(const std::string& chave) const;
    void redimensionar();
    void inserirDireto(const std::string& chave, int id);

    IndiceInvertido(const IndiceInvertido& outro);
    IndiceInvertido& operator=(const IndiceInvertido& outro);

public:
    IndiceInvertido();
    ~IndiceInvertido();

    void inicializar(int capacidadeInicial = 16);
    void destruir();
    void inserir(const std::string& chave, int id);
    const ListaIds* buscar(const std::string& chave) const;
    int getQuantidade() const;
    int getCapacidade() const;
};

#endif
