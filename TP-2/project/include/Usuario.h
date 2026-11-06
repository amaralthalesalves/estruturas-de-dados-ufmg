#ifndef USUARIO_H
#define USUARIO_H

#include <string>
// TAD Usuario - classe simples para representar um usuario, com id, nome e idade
class Usuario {
private:
    int id;
    std::string nome;
    int idade;

public:
    Usuario();
    Usuario(int id, const std::string& nome, int idade);

    int getId() const;
    const std::string& getNome() const;
    int getIdade() const;

    void setId(int id);
    void setNome(const std::string& nome);
    void setIdade(int idade);
};

#endif
