#include "Compra.h"

Compra::Compra()
    : id(-1), timestamp(0), idUsuario(-1), itens(0), numItens(0), capItens(0) {}

Compra::Compra(int id, int timestamp, int idUsuario,
               const ItemEstoque *itens, int numItens)
    : id(-1), timestamp(0), idUsuario(-1), itens(0), numItens(0), capItens(0) {
  inicializar(id, timestamp, idUsuario, itens, numItens);
}

Compra::~Compra() { destruir(); }

Compra &Compra::operator=(const Compra &outra) {
  if (this != &outra) {
    destruir();
    copiarDe(outra);
  }

  return *this;
}

void Compra::inicializar(int id, int timestamp, int idUsuario,
                         const ItemEstoque *itens, int numItens) {
  destruir();

  this->id = id;
  this->timestamp = timestamp;
  this->idUsuario = idUsuario;
  this->numItens = numItens;
  this->capItens = numItens;

  if (numItens == 0) {
    this->itens = 0;
    return;
  }

  // Os itens vem do buffer de leitura, entao a compra guarda sua propria copia
  this->itens = new ItemEstoque[capItens];
  for (int i = 0; i < numItens; i++) {
    this->itens[i] = itens[i];
  }
}

void Compra::destruir() {
  delete[] itens;

  id = -1;
  timestamp = 0;
  idUsuario = -1;
  itens = 0;
  numItens = 0;
  capItens = 0;
}

// Copia profunda pelo mesmo motivo do inicializar
void Compra::copiarDe(const Compra &outra) {
  id = outra.id;
  timestamp = outra.timestamp;
  idUsuario = outra.idUsuario;
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

int Compra::getId() const { return id; }

int Compra::getTimestamp() const { return timestamp; }

int Compra::getIdUsuario() const { return idUsuario; }

const ItemEstoque *Compra::getItens() const { return itens; }

int Compra::getNumItens() const { return numItens; }
