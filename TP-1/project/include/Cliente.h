#ifndef CLIENTE_H
#define CLIENTE_H

struct Cliente {
    int id;
    bool* carteira; //carteira[i] = true se o cliente possui a acao de id i
    int totalAcoes; //quantas acoes o cliente tem no momento

    //Inicializa o cliente com ID e carteira de tamanho N (toda false)
    void inicializar(int idCliente, int N);
    //Libera a memoria alocada para a carteira
    void destruir();
    //Compra - adiciona a acao a carteira — O(1)
    void comprar(int acao);
    //Venda - remove a acao da carteira — O(1)
    void vender(int acao);
    //Retorna true se o cliente possui a acao
    bool temAcao(int acao);
    //Retorna o numero total de acoes na carteira
    int getTotalAcoes();
};

#endif
