#include "IndiceInvertido.h"

IndiceInvertido::IndiceInvertido()
    : tabela(0),
      capacidade(0),
      quantidade(0) {}

IndiceInvertido::~IndiceInvertido() {
    destruir();
}

void IndiceInvertido::inicializar(int capacidadeInicial) {
    destruir();

    if (capacidadeInicial < 16) {
        capacidadeInicial = 16;
    }

    capacidade = capacidadeInicial;
    quantidade = 0;
    tabela = new EntradaIndice[capacidade];

    for (int i = 0; i < capacidade; i++) {
        tabela[i].ocupado = false;
    }
}

void IndiceInvertido::destruir() {
    delete[] tabela;
    tabela = 0;
    capacidade = 0;
    quantidade = 0;
}

void IndiceInvertido::inserir(const std::string& chave, int id) {
    if (capacidade == 0) {
        inicializar(16);
    }

    // Deixamos a tabela folgada para diminuir as sondagens em sequencia
    if ((quantidade + 1) * 2 >= capacidade) {
        redimensionar();
    }

    inserirDireto(chave, id);
}

// Retorna 0 quando a chave nunca apareceu naquele atributo
const ListaIds* IndiceInvertido::buscar(const std::string& chave) const {
    int posicao = buscarPosicao(chave);
    if (posicao == -1) {
        return 0;
    }

    return &tabela[posicao].ids;
}

int IndiceInvertido::calcularHash(const std::string& chave) const {
    unsigned int hash = 0;

    // Hash polinomial simples para strings. Multiplica por 31 para que a ordem das letras importe
    for (int i = 0; i < (int)chave.size(); i++) {
        hash = hash * 31 + chave[i];
    }

    return hash % capacidade;
}

int IndiceInvertido::buscarPosicao(const std::string& chave) const {
    if (capacidade == 0) {
        return -1;
    }

    int inicio = calcularHash(chave);
    int posicao = inicio;

    do {
        if (!tabela[posicao].ocupado) {
            return -1;
        }
        if (tabela[posicao].chave == chave) {
            return posicao;
        }

        // Sondagem linear: em colisao, tenta a proxima casa da tabela (simples e eficiente)
        posicao = (posicao + 1) % capacidade;
    } while (posicao != inicio);

    return -1;
}

int IndiceInvertido::buscarPosicaoInsercao(const std::string& chave) const {
    int inicio = calcularHash(chave);
    int posicao = inicio;

    do {
        if (!tabela[posicao].ocupado || tabela[posicao].chave == chave) {
            return posicao;
        }

        posicao = (posicao + 1) % capacidade;
    } while (posicao != inicio);

    return -1;
}

void IndiceInvertido::redimensionar() {
    int capacidadeAntiga = capacidade;
    EntradaIndice* tabelaAntiga = tabela;

    int novaCapacidade = capacidade * 2;
    if (novaCapacidade < 16) {
        novaCapacidade = 16;
    }

    tabela = new EntradaIndice[novaCapacidade];
    capacidade = novaCapacidade;
    quantidade = 0;

    for (int i = 0; i < capacidade; i++) {
        tabela[i].ocupado = false;
    }

    // A capacidade entra no calculo do hash, entao as posicoes antigas deixam de valer
    for (int i = 0; i < capacidadeAntiga; i++) {
        if (tabelaAntiga[i].ocupado) {
            const ListaIds& ids = tabelaAntiga[i].ids;
            for (int j = 0; j < ids.getQuantidade(); j++) {
                inserirDireto(tabelaAntiga[i].chave, ids.getId(j));
            }
        }
    }

    delete[] tabelaAntiga;
}

// Usado quando refazendo hash, qnd a capacidade ja foi preparada antes
void IndiceInvertido::inserirDireto(const std::string& chave, int id) {
    int posicao = buscarPosicaoInsercao(chave);

    if (!tabela[posicao].ocupado) {
        tabela[posicao].chave = chave;
        tabela[posicao].ocupado = true;
        quantidade++;
    }

    tabela[posicao].ids.inserir(id);
}

int IndiceInvertido::getQuantidade() const {
    return quantidade;
}

int IndiceInvertido::getCapacidade() const {
    return capacidade;
}
