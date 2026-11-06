#include "Usuario.h"

Usuario::Usuario() : id(-1), nome(""), idade(0) {}

Usuario::Usuario(int id, const std::string& nome, int idade) : id(id), nome(nome), idade(idade) {}

int Usuario::getId() const {
    return id;
}

const std::string& Usuario::getNome() const {
    return nome;
}

int Usuario::getIdade() const {
    return idade;
}

void Usuario::setId(int id) {
    this->id = id;
}

void Usuario::setNome(const std::string& nome) {
    this->nome = nome;
}

void Usuario::setIdade(int idade) {
    this->idade = idade;
}
