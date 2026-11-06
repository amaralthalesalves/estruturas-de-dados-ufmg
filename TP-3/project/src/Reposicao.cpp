#include "Reposicao.h"

Reposicao::Reposicao()
    : id(-1), timestamp(0), itens(0), numItens(0), capItens(0) {}

Reposicao::Reposicao(int id, int timestamp, const ItemEstoque *itens,
                     int numItens)
    : id(-1), timestamp(0), itens(0), numItens(0), capItens(0) {
  inicializar(id, timestamp, itens, numItens);
}

Reposicao::~Reposicao() { destruir(); }

Reposicao &Reposicao::operator=(const Reposicao &outra) {
  if (this != &outra) {
    destruir();
    copiarDe(outra);
  }

  return *this;
}

void Reposicao::inicializar(int id, int timestamp, const ItemEstoque *itens,
                            int numItens) {
  destruir();

  this->id = id;
  this->timestamp = timestamp;
  this->numItens = numItens;
  this->capItens = numItens;

  if (numItens == 0) {
    this->itens = 0;
    return;
  }

  this->itens = new ItemEstoque[capItens];
  for (int i = 0; i < numItens; i++) {
    this->itens[i] = itens[i];
  }
}

void Reposicao::destruir() {
  delete[] itens;

  id = -1;
  timestamp = 0;
  itens = 0;
  numItens = 0;
  capItens = 0;
}

// A reposicao tambem nao compartilha o vetor temporario de itens
void Reposicao::copiarDe(const Reposicao &outra) {
  id = outra.id;
  timestamp = outra.timestamp;
  numItens = outra.numItens;
  capItens = outra.capItens;

  if (capItens == 0) {
    itens = 0;
    return;
  }

  itens = new ItemEstoque[capItens];
  for (int i = 0; i < numItens; i++) {
    itens[i] = outra.itens[i];
  }
}

int Reposicao::getId() const { return id; }

int Reposicao::getTimestamp() const { return timestamp; }

const ItemEstoque *Reposicao::getItens() const { return itens; }

int Reposicao::getNumItens() const { return numItens; }
