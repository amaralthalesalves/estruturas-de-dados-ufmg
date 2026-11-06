#ifndef TEMA_H
#define TEMA_H

#include <string>
// TAD Tema - classe simples para representar um tema, com id, nome e tipo (char)
class Tema {
private:
    int id;
    std::string nome;
    char tipo; // Pode assumir {'A', 'P'}, onde 'A' = academico e 'P' = pessoal

public:
    Tema();
    Tema(int id, const std::string& nome, char tipo);

    int getId() const;
    const std::string& getNome() const;
    char getTipo() const;

    void setId(int id);
    void setNome(const std::string& nome);
    void setTipo(char tipo);
};

#endif
