#ifndef SISTEMA_H
#define SISTEMA_H

#include "BuffersConsulta.h"
#include "Compra.h"
#include "Indices.h"
#include "Produto.h"
#include "Reposicao.h"
#include "Usuario.h"

#include <string>

// Guarda o estado do e-commerce; o main interpreta os comandos e chama estes metodos
class Sistema {
private:
    Usuario* usuarios;
    int numUsuarios;
    int capUsuarios;

    Produto* produtos;
    int numProdutos;
    int capProdutos;

    Compra* compras;
    int numCompras;
    int capCompras;

    Reposicao* reposicoes;
    int numReposicoes;
    int capReposicoes;

    IndicesUsuarios indicesUsuarios;
    IndicesProdutos indicesProdutos;
    IndicesCompras indicesCompras;
    IndicesReposicoes indicesReposicoes;

    void expandirUsuarios();
    void expandirProdutos();
    void expandirCompras();
    void expandirReposicoes();

    void ordenarItensPorProduto(ItemEstoque* itens, int numItens) const;
    bool validarCompra(const ItemEstoque* itens, int numItens) const;
    void aplicarCompra(int idUsuario, const ItemEstoque* itens, int numItens);
    void aplicarReposicao(const ItemEstoque* itens, int numItens);

    // Funcoes pequenas usadas para transformar filtros em listas de ids
    int copiarIds(const ListaIds* lista, int* resultado) const;
    int copiarId(int id, int* resultado) const;
    int intersecao(const int* a, int qtdA, const int* b, int qtdB, int* resultado) const;

    int filtrarUsuarios(const std::string& atributo, const std::string& valor, int* resultado) const;
    int filtrarProdutos(const std::string& atributo, const std::string& valor, int* resultado) const;
    int filtrarCompras(const std::string& atributo, const std::string& valor, int* resultado) const;
    int filtrarReposicoes(const std::string& atributo, const std::string& valor, int* resultado) const;

    Sistema(const Sistema&);
    Sistema& operator=(const Sistema&);

public:
    Sistema(int capacidadeInicial = 8);
    ~Sistema();

    void inicializar(int capacidadeInicial = 8);
    void destruir();

    int cadastrarUsuario(const std::string& nome, int idade,
                         const std::string& cidade, const std::string& estado,
                         const std::string& nacionalidade);
    int cadastrarProduto(const std::string& nome, int precoCentavos, int qtd,
                         const std::string& categoria, const std::string& marca,
                         const std::string& condicao);

    // Compra invalida retorna -1, sem mexer em estoque, historico ou indices
    int registrarCompra(int timestamp, int idUsuario, ItemEstoque* itens, int numItens);
    int registrarReposicao(int timestamp, ItemEstoque* itens, int numItens);

    int consultarUsuarios(const std::string* atributos, const std::string* valores,
                          int numFiltros, BuffersConsulta& buffers) const;
    int consultarProdutos(const std::string* atributos, const std::string* valores,
                          int numFiltros, BuffersConsulta& buffers) const;
    int consultarCompras(const std::string* atributos, const std::string* valores,
                         int numFiltros, BuffersConsulta& buffers) const;
    int consultarReposicoes(const std::string* atributos, const std::string* valores,
                            int numFiltros, BuffersConsulta& buffers) const;

    const Usuario& getUsuario(int idUsuario) const;
    const Produto& getProduto(int idProduto) const;
    const Compra& getCompra(int idCompra) const;
    const Reposicao& getReposicao(int idReposicao) const;
    int getNumUsuarios() const;
    int getNumProdutos() const;
    int getNumCompras() const;
    int getNumReposicoes() const;
};

#endif
