#ifndef NO_H
#define NO_H

// TAD No - conteudo associado a um vertice do grafo Permite que o TAD Grafo represente usuarios e temas de forma generica
enum TipoNo {
    USUARIO,
    TEMA
};

struct No {
    int idExterno;
    TipoNo tipo;
};

#endif
