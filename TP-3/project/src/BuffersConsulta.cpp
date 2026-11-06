#include "BuffersConsulta.h"

BuffersConsulta::BuffersConsulta()
    : idsResultado(0), idsAux(0), idsFiltro(0), itensTemporarios(0),
      capacidadeIds(0), capacidadeItens(0) {}

BuffersConsulta::~BuffersConsulta() { destruir(); }

void BuffersConsulta::garantirCapacidadeIds(int novaCapacidade) {
  if (novaCapacidade <= capacidadeIds) {
    return;
  }

  int capacidadeNova = capacidadeIds;
  if (capacidadeNova < 4) {
    capacidadeNova = 4;
  }

  while (capacidadeNova < novaCapacidade) {
    capacidadeNova *= 2;
  }

  int *novosResultado = new int[capacidadeNova];
  int *novosAux = new int[capacidadeNova];
  int *novosFiltro = new int[capacidadeNova];

  for (int i = 0; i < capacidadeIds; i++) {
    novosResultado[i] = idsResultado[i];
    novosAux[i] = idsAux[i];
    novosFiltro[i] = idsFiltro[i];
  }

  delete[] idsResultado;
  delete[] idsAux;
  delete[] idsFiltro;

  idsResultado = novosResultado;
  idsAux = novosAux;
  idsFiltro = novosFiltro;
  capacidadeIds = capacidadeNova;
}

void BuffersConsulta::garantirCapacidadeItens(int novaCapacidade) {
  if (novaCapacidade <= capacidadeItens) {
    return;
  }

  int capacidadeNova = capacidadeItens;
  if (capacidadeNova < 4) {
    capacidadeNova = 4;
  }

  while (capacidadeNova < novaCapacidade) {
    capacidadeNova *= 2;
  }

  ItemEstoque *novosItens = new ItemEstoque[capacidadeNova];

  for (int i = 0; i < capacidadeItens; i++) {
    novosItens[i] = itensTemporarios[i];
  }

  delete[] itensTemporarios;

  itensTemporarios = novosItens;
  capacidadeItens = capacidadeNova;
}

void BuffersConsulta::destruir() {
  delete[] idsResultado;
  delete[] idsAux;
  delete[] idsFiltro;
  delete[] itensTemporarios;

  idsResultado = 0;
  idsAux = 0;
  idsFiltro = 0;
  itensTemporarios = 0;
  capacidadeIds = 0;
  capacidadeItens = 0;
}
