#include "Acao.h"
#include <cmath>

//Inicializador das acoes - aloca memoria para as cotacoes e inicializa o ID de acordo com input
void Acao::inicializar(int idAcao, int w) {
    id = idAcao;
    cotacoes = new double[w];
    inicio = 0;
    quantidade = 0;
    capacidade = w;
}

//Destrutor das acoes - libera memoria alocada para as cotacoes
void Acao::destruir() {
    if (cotacoes != nullptr) {
        delete[] cotacoes;
        cotacoes = nullptr;
    }
}

//Registra uma nova cotacao no array de forma circular - se o array estiver cheio, substitui a cotacao mais antiga
//e marca o inicio como sendo a proxima posicao
void Acao::registrarCotacao(double preco) {
    cotacoes[(inicio + quantidade) % capacidade] = preco;
    if (quantidade < capacidade) {
        quantidade++;
    } else {
        inicio = (inicio + 1) % capacidade;
    }
}

//Retorna a cotacao de acordo com o indice da mais antiga a mais recente
double Acao::getCotacao(int i) {
    return cotacoes[(inicio + i) % capacidade];
}

int Acao::getQuantidade() {
    return quantidade;
}

double Acao::calcularMetrica(int metrica) {
    const double SENTINELA = -1e18;
    //Se a cotacao nao estiver completa, retorna SENTINELA porque as metricas nao podem ser calculadas corretamente
    if (quantidade < capacidade)
        return SENTINELA;
    //Copia as cotacoes para um array local para listar do inicio ao fim (sem necessidade do modulo)
    double p[capacidade];
    for (int i = 0; i < capacidade; i++)
        p[i] = getCotacao(i);
    //Calcula os retornos elementares
    double ret_elem[capacidade];
    for (int i = 1; i < capacidade; i++)
        ret_elem[i] = p[i] / p[i - 1] - 1.0;
    //Calcula a metrica de acordo com o tipo de metrica - 0: RET, 1: AVGRET, 2: STAB, 3: CONS
    switch (metrica) {
        case 0: { // RET - retorno total, do p[0] ao p[w-1]
            return p[capacidade - 1] / p[0] - 1.0;
        }
        case 1: { // AVGRET - media dos retornos elementares do p[0] ao p[w-1]
            double soma = 0.0;
            for (int i = 1; i < capacidade; i++) soma += ret_elem[i];
            return soma / (capacidade - 1);
        }
        case 2: { // STAB - estabilidade da acao, calculada pela formula 1 / (1 + vol)
            double rbar = 0.0;
            for (int i = 1; i < capacidade; i++) rbar += ret_elem[i];
            rbar /= (capacidade - 1);
            double var = 0.0;
            for (int i = 1; i < capacidade; i++)
                var += (ret_elem[i] - rbar) * (ret_elem[i] - rbar);
            var /= (capacidade - 1);
            double vol = sqrt(var);
            return 1.0 / (1.0 + vol);
        }
        case 3: { // CONS - contagem de retornos positivos do p[0] ao p[w-1]
            int posCount = 0;
            for (int i = 1; i < capacidade; i++)
                if (ret_elem[i] > 0) posCount++;
            return (double)posCount / (capacidade - 1);
        }
    }
    return SENTINELA;
}
