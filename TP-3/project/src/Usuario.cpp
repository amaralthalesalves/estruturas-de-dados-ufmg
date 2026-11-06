#include "Usuario.h"

Usuario::Usuario()
    : id(-1), idade(0), produtosComprados(0), numProdutosComprados(0),
      capProdutosComprados(0) {}

Usuario::Usuario(int id, const std::string &nome, int idade,
                 const std::string &cidade, const std::string &estado,
                 const std::string &nacionalidade)
    : id(-1), idade(0), produtosComprados(0), numProdutosComprados(0),
      capProdutosComprados(0) {
  inicializar(id, nome, idade, cidade, estado, nacionalidade);
}

Usuario::~Usuario() { destruir(); }

Usuario &Usuario::operator=(const Usuario &outro) {
  if (this != &outro) {
    destruir();
    copiarDe(outro);
  }

  return *this;
}

void Usuario::inicializar(int id, const std::string &nome, int idade,
                          const std::string &cidade, const std::string &estado,
                          const std::string &nacionalidade) {
  destruir();

  this->id = id;
  this->nome = nome;
  this->idade = idade;
  this->cidade = cidade;
  this->estado = estado;
  this->nacionalidade = nacionalidade;
}

void Usuario::destruir() {
  delete[] produtosComprados;

  id = -1;
  nome = "";
  idade = 0;
  cidade = "";
  estado = "";
  nacionalidade = "";
  produtosComprados = 0;
  numProdutosComprados = 0;
  capProdutosComprados = 0;
}

void Usuario::registrarProdutoComprado(int idProduto, int quantidade) {
  int posicao = buscarProdutoComprado(idProduto);
  if (posicao != -1) {
    produtosComprados[posicao].qtdComprada += quantidade;
    return;
  }

  if (numProdutosComprados == capProdutosComprados) {
    expandirProdutosComprados();
  }

  // Mantemos ordenado para nao precisar ordenar o historico a cada consulta LU
  int posicaoInsercao = numProdutosComprados;
  while (posicaoInsercao > 0 &&
         produtosComprados[posicaoInsercao - 1].idProduto > idProduto) {
    produtosComprados[posicaoInsercao] = produtosComprados[posicaoInsercao - 1];
    posicaoInsercao--;
  }

  produtosComprados[posicaoInsercao].idProduto = idProduto;
  produtosComprados[posicaoInsercao].qtdComprada = quantidade;
  numProdutosComprados++;
}

void Usuario::expandirProdutosComprados() {
  int novaCapacidade = capProdutosComprados * 2;
  if (novaCapacidade < 4) {
    novaCapacidade = 4;
  }

  ProdutoComprado *novosProdutos = new ProdutoComprado[novaCapacidade];
  for (int i = 0; i < numProdutosComprados; i++) {
    novosProdutos[i] = produtosComprados[i];
  }

  delete[] produtosComprados;
  produtosComprados = novosProdutos;
  capProdutosComprados = novaCapacidade;
}

int Usuario::buscarProdutoComprado(int idProduto) const {
  for (int i = 0; i < numProdutosComprados; i++) {
    if (produtosComprados[i].idProduto == idProduto) {
      return i;
    }
    if (produtosComprados[i].idProduto > idProduto) {
      return -1;
    }
  }

  return -1;
}

// Copia profunda porque o historico fica em vetor dinamico
void Usuario::copiarDe(const Usuario &outro) {
  id = outro.id;
  nome = outro.nome;
  idade = outro.idade;
  cidade = outro.cidade;
  estado = outro.estado;
  nacionalidade = outro.nacionalidade;
  numProdutosComprados = outro.numProdutosComprados;
  capProdutosComprados = outro.capProdutosComprados;

  if (capProdutosComprados == 0) {
    produtosComprados = 0;
    return;
  }

  produtosComprados = new ProdutoComprado[capProdutosComprados];
  for (int i = 0; i < numProdutosComprados; i++) {
    produtosComprados[i] = outro.produtosComprados[i];
  }
}

int Usuario::getId() const { return id; }

const std::string &Usuario::getNome() const { return nome; }

int Usuario::getIdade() const { return idade; }

const std::string &Usuario::getCidade() const { return cidade; }

const std::string &Usuario::getEstado() const { return estado; }

const std::string &Usuario::getNacionalidade() const { return nacionalidade; }

const ProdutoComprado *Usuario::getProdutosComprados() const {
  return produtosComprados;
}

int Usuario::getNumProdutosComprados() const { return numProdutosComprados; }
