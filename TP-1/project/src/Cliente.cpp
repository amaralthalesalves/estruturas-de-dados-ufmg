#include "Cliente.h"

//Inicializa o cliente com ID e carteira de tamanho n vazia
void Cliente::inicializar(int idCliente, int n) {
    id = idCliente;
    carteira = new bool[n];
    for (int i = 0; i < n; i++)
        carteira[i] = false;
    totalAcoes = 0;
}

//Libera a memoria alocada para a carteira
void Cliente::destruir() {
    delete[] carteira;
    carteira = nullptr;
}

//Compra - adiciona a acao a carteira e incrementa o total apenas se ainda nao a possui
void Cliente::comprar(int acao) {
    if (!carteira[acao]) {
        carteira[acao] = true;
        totalAcoes++;
    }
}

//Venda - remove a acao da carteira e decresce o total apenas se possuia a acao
void Cliente::vender(int acao) {
    if (carteira[acao]) {
        carteira[acao] = false;
        totalAcoes--;
    }
}

//Retorna true se o cliente possui a acao
bool Cliente::temAcao(int acao) {
    return carteira[acao];
}

//Retorna o numero total de acoes na carteira
int Cliente::getTotalAcoes() {
    return totalAcoes;
}
