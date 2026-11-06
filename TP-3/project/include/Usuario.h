#ifndef USUARIO_H
#define USUARIO_H

#include "Tipos.h"

#include <string>

// Usuario guarda seus dados fixos e um historico agregado para a saida de LU
class Usuario {
private:
    int id;
    std::string nome;
    int idade;
    std::string cidade;
    std::string estado;
    std::string nacionalidade;

    ProdutoComprado* produtosComprados;
    int numProdutosComprados;
    int capProdutosComprados;

    void expandirProdutosComprados();
    int buscarProdutoComprado(int idProduto) const;
    void copiarDe(const Usuario& outro);

    Usuario(const Usuario& outro);

public:
    Usuario();
    Usuario(int id, const std::string& nome, int idade,
            const std::string& cidade, const std::string& estado,
            const std::string& nacionalidade);
    ~Usuario();

    Usuario& operator=(const Usuario& outro);

    void inicializar(int id, const std::string& nome, int idade,
                     const std::string& cidade, const std::string& estado,
                     const std::string& nacionalidade);
    void destruir();
    void registrarProdutoComprado(int idProduto, int quantidade);

    int getId() const;
    const std::string& getNome() const;
    int getIdade() const;
    const std::string& getCidade() const;
    const std::string& getEstado() const;
    const std::string& getNacionalidade() const;
    const ProdutoComprado* getProdutosComprados() const;
    int getNumProdutosComprados() const;
};

#endif
