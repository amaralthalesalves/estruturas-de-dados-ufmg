#ifndef DICIONARIO_H
#define DICIONARIO_H

#include "Tema.h"
#include "Usuario.h"
// TAD Dicionario - armazena usuarios e temas e mapeia os IDs externos para os indices internos dos grafos
class Dicionario {
private:
    Usuario* usuarios;
    int numUsuarios;
    int capUsuarios;

    Tema* temas;
    int numTemas;
    int capTemas;

    int* indUsuarioSocial;  // Mapeia idUsuario para indice interno no grafo social
    int* indUsuarioTemas;   // Mapeia idUsuario para indice interno no grafo de temas
    int* indTemaTemas;      // Mapeia idTema para indice interno no grafo de temas

    void inicializar(int capacidadeUsuarios, int capacidadeTemas);
    // Expande a capacidade dos vetores
    void expandirUsuarios();
    void expandirTemas();

    Dicionario(const Dicionario&);
    Dicionario& operator=(const Dicionario&);

public:
    Dicionario(int capacidadeUsuarios = 8, int capacidadeTemas = 8);
    ~Dicionario();

    int registrarUsuario(const Usuario& usuario, int indiceNoSocial, int indiceNoTemas);
    int registrarTema(const Tema& tema, int indiceNoTemas);

    // Getters padrao
    Usuario& getUsuario(int idUsuario);
    const Usuario& getUsuario(int idUsuario) const;

    Tema& getTema(int idTema);
    const Tema& getTema(int idTema) const;

    int getIndUsuarioSocial(int idUsuario) const;
    int getIndUsuarioTemas(int idUsuario) const;
    int getIndTemaTemas(int idTema) const;

    int getNumUsuarios() const;
    int getNumTemas() const;
};

#endif
