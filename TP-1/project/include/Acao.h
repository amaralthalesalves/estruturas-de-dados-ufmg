#ifndef ACAO_H
#define ACAO_H

struct Acao {
    int id;
    double* cotacoes;
    int inicio;
    int quantidade;
    int capacidade;

    //Inicializador das acoes - aloca memoria para as cotacoes e inicializa o ID de acordo com input
    void inicializar(int idAcao, int w);
    //Destrutor das acoes - libera memoria alocada para as cotacoes
    void destruir();
    //Registra uma nova cotacao no array de forma circular - se o array estiver cheio, substitui a cotacao mais antiga
    //e marca o inicio como sendo a proxima posicao
    void registrarCotacao(double preco);
    //Retorna a cotacao de acordo com o indice da mais antiga a mais recente
    double getCotacao(int i);
    int getQuantidade();
    double calcularMetrica(int metrica);
};

#endif