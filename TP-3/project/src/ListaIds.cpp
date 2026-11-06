#include "ListaIds.h"

ListaIds::ListaIds() : ids(0), quantidade(0), capacidade(0) {}

ListaIds::~ListaIds() { destruir(); }

void ListaIds::inicializar(int capacidadeInicial) {
  destruir();

  if (capacidadeInicial < 4) {
    capacidadeInicial = 4;
  }

  ids = new int[capacidadeInicial];
  quantidade = 0;
  capacidade = capacidadeInicial;
}

void ListaIds::destruir() {
  delete[] ids;
  ids = 0;
  quantidade = 0;
  capacidade = 0;
}

void ListaIds::inserir(int id) {
  if (capacidade == 0) {
    inicializar(4);
  }

  if (quantidade == capacidade) {
    expandir();
  }

  ids[quantidade] = id;
  quantidade++;
}

void ListaIds::expandir() {
  int novaCapacidade = capacidade * 2;
  if (novaCapacidade < 4) {
    novaCapacidade = 4;
  }

  int *novosIds = new int[novaCapacidade];
  for (int i = 0; i < quantidade; i++) {
    novosIds[i] = ids[i];
  }

  delete[] ids;
  ids = novosIds;
  capacidade = novaCapacidade;
}

int ListaIds::getId(int posicao) const { return ids[posicao]; }

int ListaIds::getQuantidade() const { return quantidade; }

int ListaIds::getCapacidade() const { return capacidade; }
