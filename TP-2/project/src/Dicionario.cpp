#include "Dicionario.h"

Dicionario::Dicionario(int capacidadeUsuarios, int capacidadeTemas)
    : usuarios(0),
      numUsuarios(0),
      capUsuarios(0),
      temas(0),
      numTemas(0),
      capTemas(0),
      indUsuarioSocial(0),
      indUsuarioTemas(0),
      indTemaTemas(0) {
    inicializar(capacidadeUsuarios, capacidadeTemas);
}

Dicionario::~Dicionario() {
    delete[] usuarios;
    delete[] temas;
    delete[] indUsuarioSocial;
    delete[] indUsuarioTemas;
    delete[] indTemaTemas;
}

// Inicializa o dicionario alocando os vetores e preenchendo os mapeamentos com -1
void Dicionario::inicializar(int capacidadeUsuarios, int capacidadeTemas) {
    if (capacidadeUsuarios < 1) {
        capacidadeUsuarios = 8;
    }
    if (capacidadeTemas < 1) {
        capacidadeTemas = 8;
    }

    capUsuarios = capacidadeUsuarios;
    capTemas = capacidadeTemas;

    usuarios = new Usuario[capUsuarios];
    temas = new Tema[capTemas];

    indUsuarioSocial = new int[capUsuarios];
    indUsuarioTemas = new int[capUsuarios];
    indTemaTemas = new int[capTemas];

    for (int i = 0; i < capUsuarios; i++) {
        indUsuarioSocial[i] = -1;
        indUsuarioTemas[i] = -1;
    }

    for (int i = 0; i < capTemas; i++) {
        indTemaTemas[i] = -1;
    }
}
// Dobra a capacidade o vetor de usuarios e os mapeamentos relacionados, copiando os dados antigos para os novos vetores
void Dicionario::expandirUsuarios() {
    int novaCapacidade = capUsuarios * 2;

    Usuario* novosUsuarios = new Usuario[novaCapacidade];
    int* novoIndUsuarioSocial = new int[novaCapacidade];
    int* novoIndUsuarioTemas = new int[novaCapacidade];

    for (int i = 0; i < numUsuarios; i++) {
        novosUsuarios[i] = usuarios[i];
        novoIndUsuarioSocial[i] = indUsuarioSocial[i];
        novoIndUsuarioTemas[i] = indUsuarioTemas[i];
    }
    // As novas posicoes ainda nao correspondem a usuarios cadastrados
    for (int i = numUsuarios; i < novaCapacidade; i++) {
        novoIndUsuarioSocial[i] = -1;
        novoIndUsuarioTemas[i] = -1;
    }

    delete[] usuarios;
    delete[] indUsuarioSocial;
    delete[] indUsuarioTemas;

    usuarios = novosUsuarios;
    indUsuarioSocial = novoIndUsuarioSocial;
    indUsuarioTemas = novoIndUsuarioTemas;
    capUsuarios = novaCapacidade;
}
// Dobra a capacidade dos temas da mesma forma q foi feito com usuarios
void Dicionario::expandirTemas() {
    int novaCapacidade = capTemas * 2;

    Tema* novosTemas = new Tema[novaCapacidade];
    int* novoIndTemaTemas = new int[novaCapacidade];

    for (int i = 0; i < numTemas; i++) {
        novosTemas[i] = temas[i];
        novoIndTemaTemas[i] = indTemaTemas[i];
    }

    for (int i = numTemas; i < novaCapacidade; i++) {
        novoIndTemaTemas[i] = -1;
    }

    delete[] temas;
    delete[] indTemaTemas;

    temas = novosTemas;
    indTemaTemas = novoIndTemaTemas;
    capTemas = novaCapacidade;
}
// Registra o usuario na proxima posicao livre, o contador interno define o ID e a posicao no vetor
int Dicionario::registrarUsuario(const Usuario& usuario, int indiceNoSocial, int indiceNoTemas) {
    // Expande o vetor de usuarios caso a capacidade atual seja atingida
    if (numUsuarios == capUsuarios) {
        expandirUsuarios();
    }

    int idUsuario = numUsuarios;
    usuarios[idUsuario] = usuario;
    indUsuarioSocial[idUsuario] = indiceNoSocial;
    indUsuarioTemas[idUsuario] = indiceNoTemas;

    numUsuarios++;
    return idUsuario;
}
// Registra tema na proxima posicao livre e guarda seu indice no grafo de temas
int Dicionario::registrarTema(const Tema& tema, int indiceNoTemas) {
    // Expande o vetor de temas caso a capacidade atual seja atingida
    if (numTemas == capTemas) {
        expandirTemas();
    }

    int idTema = numTemas;
    temas[idTema] = tema;
    indTemaTemas[idTema] = indiceNoTemas;

    numTemas++;
    return idTema;
}

// Getters e setters basicos
Usuario& Dicionario::getUsuario(int idUsuario) {
    return usuarios[idUsuario];
}

const Usuario& Dicionario::getUsuario(int idUsuario) const {
    return usuarios[idUsuario];
}

Tema& Dicionario::getTema(int idTema) {
    return temas[idTema];
}

const Tema& Dicionario::getTema(int idTema) const {
    return temas[idTema];
}

int Dicionario::getIndUsuarioSocial(int idUsuario) const {
    return indUsuarioSocial[idUsuario];
}

int Dicionario::getIndUsuarioTemas(int idUsuario) const {
    return indUsuarioTemas[idUsuario];
}

int Dicionario::getIndTemaTemas(int idTema) const {
    return indTemaTemas[idTema];
}

int Dicionario::getNumUsuarios() const {
    return numUsuarios;
}

int Dicionario::getNumTemas() const {
    return numTemas;
}
