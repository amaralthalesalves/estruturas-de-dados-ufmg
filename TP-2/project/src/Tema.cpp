#include "Tema.h"

Tema::Tema() : id(-1), nome(""), tipo('\0') {}

Tema::Tema(int id, const std::string& nome, char tipo)
    : id(id), nome(nome), tipo(tipo) {}

int Tema::getId() const {
    return id;
}

const std::string& Tema::getNome() const {
    return nome;
}

char Tema::getTipo() const {
    return tipo;
}

void Tema::setId(int id) {
    this->id = id;
}

void Tema::setNome(const std::string& nome) {
    this->nome = nome;
}

void Tema::setTipo(char tipo) {
    this->tipo = tipo;
}
