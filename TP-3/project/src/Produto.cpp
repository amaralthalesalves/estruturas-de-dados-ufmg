#include "Produto.h"

Produto::Produto()
    : id(-1), precoCentavos(0), qtd(0), usuariosCompradores(0),
      numUsuariosCompradores(0), capUsuariosCompradores(0) {}

Produto::Produto(int id, const std::string &nome, int precoCentavos, int qtd,
                 const std::string &categoria, const std::string &marca,
                 const std::string &condicao)
    : id(-1), precoCentavos(0), qtd(0), usuariosCompradores(0),
      numUsuariosCompradores(0), capUsuariosCompradores(0) {
  inicializar(id, nome, precoCentavos, qtd, categoria, marca, condicao);
}

Produto::~Produto() { destruir(); }

Produto &Produto::operator=(const Produto &outro) {
  if (this != &outro) {
    destruir();
    copiarDe(outro);
  }

  return *this;
}

void Produto::inicializar(int id, const std::string &nome, int precoCentavos,
                          int qtd, const std::string &categoria,
                          const std::string &marca,
                          const std::string &condicao) {
  destruir();

  this->id = id;
  this->nome = nome;
  this->precoCentavos = precoCentavos;
  this->qtd = qtd;
  this->categoria = categoria;
  this->marca = marca;
  this->condicao = condicao;
}

void Produto::destruir() {
  delete[] usuariosCompradores;

  id = -1;
  nome = "";
  precoCentavos = 0;
  qtd = 0;
  categoria = "";
  marca = "";
  condicao = "";
  usuariosCompradores = 0;
  numUsuariosCompradores = 0;
  capUsuariosCompradores = 0;
}

void Produto::incrementarEstoque(int quantidade) { qtd += quantidade; }

bool Produto::temEstoque(int quantidade) const { return qtd >= quantidade; }

void Produto::decrementarEstoque(int quantidade) { qtd -= quantidade; }

void Produto::registrarUsuarioComprador(int idUsuario, int quantidade) {
  int posicao = buscarUsuarioComprador(idUsuario);
  if (posicao != -1) {
    usuariosCompradores[posicao].qtdComprada += quantidade;
    return;
  }

  if (numUsuariosCompradores == capUsuariosCompradores) {
    expandirUsuariosCompradores();
  }

  // O historico ja fica na ordem esperada pela consulta LP
  int posicaoInsercao = numUsuariosCompradores;
  while (posicaoInsercao > 0 &&
         usuariosCompradores[posicaoInsercao - 1].idUsuario > idUsuario) {
    usuariosCompradores[posicaoInsercao] =
        usuariosCompradores[posicaoInsercao - 1];
    posicaoInsercao--;
  }

  usuariosCompradores[posicaoInsercao].idUsuario = idUsuario;
  usuariosCompradores[posicaoInsercao].qtdComprada = quantidade;
  numUsuariosCompradores++;
}

void Produto::expandirUsuariosCompradores() {
  int novaCapacidade = capUsuariosCompradores * 2;
  if (novaCapacidade < 4) {
    novaCapacidade = 4;
  }

  UsuarioComprador *novosUsuarios = new UsuarioComprador[novaCapacidade];
  for (int i = 0; i < numUsuariosCompradores; i++) {
    novosUsuarios[i] = usuariosCompradores[i];
  }

  delete[] usuariosCompradores;
  usuariosCompradores = novosUsuarios;
  capUsuariosCompradores = novaCapacidade;
}

int Produto::buscarUsuarioComprador(int idUsuario) const {
  for (int i = 0; i < numUsuariosCompradores; i++) {
    if (usuariosCompradores[i].idUsuario == idUsuario) {
      return i;
    }
    if (usuariosCompradores[i].idUsuario > idUsuario) {
      return -1;
    }
  }

  return -1;
}

void Produto::copiarDe(const Produto &outro) {
  id = outro.id;
  nome = outro.nome;
  precoCentavos = outro.precoCentavos;
  qtd = outro.qtd;
  categoria = outro.categoria;
  marca = outro.marca;
  condicao = outro.condicao;
  numUsuariosCompradores = outro.numUsuariosCompradores;
  capUsuariosCompradores = outro.capUsuariosCompradores;

  if (capUsuariosCompradores == 0) {
    usuariosCompradores = 0;
    return;
  }

  usuariosCompradores = new UsuarioComprador[capUsuariosCompradores];
  for (int i = 0; i < numUsuariosCompradores; i++) {
    usuariosCompradores[i] = outro.usuariosCompradores[i];
  }
}

int Produto::getId() const { return id; }

const std::string &Produto::getNome() const { return nome; }

int Produto::getPrecoCentavos() const { return precoCentavos; }

int Produto::getQtd() const { return qtd; }

const std::string &Produto::getCategoria() const { return categoria; }

const std::string &Produto::getMarca() const { return marca; }

const std::string &Produto::getCondicao() const { return condicao; }

const UsuarioComprador *Produto::getUsuariosCompradores() const {
  return usuariosCompradores;
}

int Produto::getNumUsuariosCompradores() const {
  return numUsuariosCompradores;
}
