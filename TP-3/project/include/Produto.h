#ifndef PRODUTO_H
#define PRODUTO_H

#include "Tipos.h"

#include <string>

// Produto concentra estoque atual e os compradores agregados para a saida de LP
class Produto {
private:
    int id;
    std::string nome;
    int precoCentavos;
    int qtd;
    std::string categoria;
    std::string marca;
    std::string condicao;

    UsuarioComprador* usuariosCompradores;
    int numUsuariosCompradores;
    int capUsuariosCompradores;

    void expandirUsuariosCompradores();
    int buscarUsuarioComprador(int idUsuario) const;
    void copiarDe(const Produto& outro);

    Produto(const Produto& outro);

public:
    Produto();
    Produto(int id, const std::string& nome, int precoCentavos, int qtd,
            const std::string& categoria, const std::string& marca,
            const std::string& condicao);
    ~Produto();

    Produto& operator=(const Produto& outro);

    void inicializar(int id, const std::string& nome, int precoCentavos, int qtd,
                     const std::string& categoria, const std::string& marca,
                     const std::string& condicao);
    void destruir();
    void incrementarEstoque(int quantidade);
    bool temEstoque(int quantidade) const;
    void decrementarEstoque(int quantidade);
    void registrarUsuarioComprador(int idUsuario, int quantidade);

    int getId() const;
    const std::string& getNome() const;
    int getPrecoCentavos() const;
    int getQtd() const;
    const std::string& getCategoria() const;
    const std::string& getMarca() const;
    const std::string& getCondicao() const;
    const UsuarioComprador* getUsuariosCompradores() const;
    int getNumUsuariosCompradores() const;
};

#endif
