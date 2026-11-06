#include "Sistema.h"

#include "Auxiliares.h"

#include <cstdlib>

Sistema::Sistema(int capacidadeInicial)
    : usuarios(0), numUsuarios(0), capUsuarios(0), produtos(0), numProdutos(0),
      capProdutos(0), compras(0), numCompras(0), capCompras(0), reposicoes(0),
      numReposicoes(0), capReposicoes(0) {
  inicializar(capacidadeInicial);
}

Sistema::~Sistema() { destruir(); }

void Sistema::inicializar(int capacidadeInicial) {
  destruir();

  if (capacidadeInicial < 8) {
    capacidadeInicial = 8;
  }

  usuarios = new Usuario[capacidadeInicial];
  produtos = new Produto[capacidadeInicial];
  compras = new Compra[capacidadeInicial];
  reposicoes = new Reposicao[capacidadeInicial];

  capUsuarios = capacidadeInicial;
  capProdutos = capacidadeInicial;
  capCompras = capacidadeInicial;
  capReposicoes = capacidadeInicial;

  indicesUsuarios.inicializar();
  indicesProdutos.inicializar();
  indicesCompras.inicializar();
  indicesReposicoes.inicializar();
}

void Sistema::destruir() {
  delete[] usuarios;
  delete[] produtos;
  delete[] compras;
  delete[] reposicoes;

  usuarios = 0;
  produtos = 0;
  compras = 0;
  reposicoes = 0;

  numUsuarios = 0;
  numProdutos = 0;
  numCompras = 0;
  numReposicoes = 0;

  capUsuarios = 0;
  capProdutos = 0;
  capCompras = 0;
  capReposicoes = 0;

  indicesUsuarios.destruir();
  indicesProdutos.destruir();
  indicesCompras.destruir();
  indicesReposicoes.destruir();
}

int Sistema::cadastrarUsuario(const std::string &nome, int idade,
                              const std::string &cidade,
                              const std::string &estado,
                              const std::string &nacionalidade) {
  if (numUsuarios == capUsuarios) {
    expandirUsuarios();
  }

  int idUsuario = numUsuarios;
  usuarios[idUsuario].inicializar(idUsuario, nome, idade, cidade, estado,
                                  nacionalidade);
  numUsuarios++;

  indicesUsuarios.nome.inserir(nome, idUsuario);
  indicesUsuarios.idade.inserir(chaveInteiro(idade), idUsuario);
  indicesUsuarios.cidade.inserir(cidade, idUsuario);
  indicesUsuarios.estado.inserir(estado, idUsuario);
  indicesUsuarios.nacionalidade.inserir(nacionalidade, idUsuario);

  return idUsuario;
}

int Sistema::cadastrarProduto(const std::string &nome, int precoCentavos,
                              int qtd, const std::string &categoria,
                              const std::string &marca,
                              const std::string &condicao) {
  if (numProdutos == capProdutos) {
    expandirProdutos();
  }

  int idProduto = numProdutos;
  produtos[idProduto].inicializar(idProduto, nome, precoCentavos, qtd,
                                  categoria, marca, condicao);
  numProdutos++;

  indicesProdutos.nome.inserir(nome, idProduto);
  indicesProdutos.categoria.inserir(categoria, idProduto);
  indicesProdutos.marca.inserir(marca, idProduto);
  indicesProdutos.condicao.inserir(condicao, idProduto);

  return idProduto;
}

int Sistema::registrarCompra(int timestamp, int idUsuario, ItemEstoque *itens,
                             int numItens) {
  // A entrada pode trazer produtos fora de ordem; guardamos ordenado para imprimir depois
  ordenarItensPorProduto(itens, numItens);

  // O enunciado exige que compra invalida nao consuma id nem altere o estado
  if (!validarCompra(itens, numItens)) {
    return -1;
  }

  if (numCompras == capCompras) {
    expandirCompras();
  }

  int idCompra = numCompras;
  aplicarCompra(idUsuario, itens, numItens);
  compras[idCompra].inicializar(idCompra, timestamp, idUsuario, itens,
                                numItens);
  numCompras++;

  indicesCompras.timestamp.inserir(chaveInteiro(timestamp), idCompra);
  indicesCompras.usuario.inserir(chaveInteiro(idUsuario), idCompra);
  for (int i = 0; i < numItens; i++) {
    indicesCompras.produto.inserir(chaveInteiro(itens[i].idProduto), idCompra);
  }

  return idCompra;
}

int Sistema::registrarReposicao(int timestamp, ItemEstoque *itens,
                                int numItens) {
  // Mesma convencao das compras, assim a saida nao depende da ordem da entrada
  ordenarItensPorProduto(itens, numItens);

  if (numReposicoes == capReposicoes) {
    expandirReposicoes();
  }

  int idReposicao = numReposicoes;
  aplicarReposicao(itens, numItens);
  reposicoes[idReposicao].inicializar(idReposicao, timestamp, itens, numItens);
  numReposicoes++;

  indicesReposicoes.timestamp.inserir(chaveInteiro(timestamp), idReposicao);
  for (int i = 0; i < numItens; i++) {
    indicesReposicoes.produto.inserir(chaveInteiro(itens[i].idProduto),
                                      idReposicao);
  }

  return idReposicao;
}

int Sistema::consultarUsuarios(const std::string *atributos,
                               const std::string *valores, int numFiltros,
                               BuffersConsulta &buffers) const {
  buffers.garantirCapacidadeIds(numUsuarios);

  if (numFiltros == 0) {
    for (int i = 0; i < numUsuarios; i++) {
      buffers.idsResultado[i] = i;
    }
    return numUsuarios;
  }

  int qtdResultados =
      filtrarUsuarios(atributos[0], valores[0], buffers.idsResultado);

  // Como os ids entram nos indices em ordem de cadastro, a intersecao pode ser linear
  for (int i = 1; i < numFiltros && qtdResultados > 0; i++) {
    int qtdFiltrados =
        filtrarUsuarios(atributos[i], valores[i], buffers.idsFiltro);
    int qtdComuns =
        intersecao(buffers.idsResultado, qtdResultados, buffers.idsFiltro,
                   qtdFiltrados, buffers.idsAux);

    for (int j = 0; j < qtdComuns; j++) {
      buffers.idsResultado[j] = buffers.idsAux[j];
    }
    qtdResultados = qtdComuns;
  }

  return qtdResultados;
}

int Sistema::consultarProdutos(const std::string *atributos,
                               const std::string *valores, int numFiltros,
                               BuffersConsulta &buffers) const {
  buffers.garantirCapacidadeIds(numProdutos);

  if (numFiltros == 0) {
    for (int i = 0; i < numProdutos; i++) {
      buffers.idsResultado[i] = i;
    }
    return numProdutos;
  }

  int qtdResultados =
      filtrarProdutos(atributos[0], valores[0], buffers.idsResultado);

  for (int i = 1; i < numFiltros && qtdResultados > 0; i++) {
    int qtdFiltrados =
        filtrarProdutos(atributos[i], valores[i], buffers.idsFiltro);
    int qtdComuns =
        intersecao(buffers.idsResultado, qtdResultados, buffers.idsFiltro,
                   qtdFiltrados, buffers.idsAux);

    for (int j = 0; j < qtdComuns; j++) {
      buffers.idsResultado[j] = buffers.idsAux[j];
    }
    qtdResultados = qtdComuns;
  }

  return qtdResultados;
}

int Sistema::consultarCompras(const std::string *atributos,
                              const std::string *valores, int numFiltros,
                              BuffersConsulta &buffers) const {
  buffers.garantirCapacidadeIds(numCompras);

  if (numFiltros == 0) {
    for (int i = 0; i < numCompras; i++) {
      buffers.idsResultado[i] = i;
    }
    return numCompras;
  }

  int qtdResultados =
      filtrarCompras(atributos[0], valores[0], buffers.idsResultado);

  for (int i = 1; i < numFiltros && qtdResultados > 0; i++) {
    int qtdFiltrados =
        filtrarCompras(atributos[i], valores[i], buffers.idsFiltro);
    int qtdComuns =
        intersecao(buffers.idsResultado, qtdResultados, buffers.idsFiltro,
                   qtdFiltrados, buffers.idsAux);

    for (int j = 0; j < qtdComuns; j++) {
      buffers.idsResultado[j] = buffers.idsAux[j];
    }
    qtdResultados = qtdComuns;
  }

  return qtdResultados;
}

int Sistema::consultarReposicoes(const std::string *atributos,
                                 const std::string *valores, int numFiltros,
                                 BuffersConsulta &buffers) const {
  buffers.garantirCapacidadeIds(numReposicoes);

  if (numFiltros == 0) {
    for (int i = 0; i < numReposicoes; i++) {
      buffers.idsResultado[i] = i;
    }
    return numReposicoes;
  }

  int qtdResultados =
      filtrarReposicoes(atributos[0], valores[0], buffers.idsResultado);

  for (int i = 1; i < numFiltros && qtdResultados > 0; i++) {
    int qtdFiltrados =
        filtrarReposicoes(atributos[i], valores[i], buffers.idsFiltro);
    int qtdComuns =
        intersecao(buffers.idsResultado, qtdResultados, buffers.idsFiltro,
                   qtdFiltrados, buffers.idsAux);

    for (int j = 0; j < qtdComuns; j++) {
      buffers.idsResultado[j] = buffers.idsAux[j];
    }
    qtdResultados = qtdComuns;
  }

  return qtdResultados;
}

const Usuario &Sistema::getUsuario(int idUsuario) const {
  return usuarios[idUsuario];
}

const Produto &Sistema::getProduto(int idProduto) const {
  return produtos[idProduto];
}

const Compra &Sistema::getCompra(int idCompra) const {
  return compras[idCompra];
}

const Reposicao &Sistema::getReposicao(int idReposicao) const {
  return reposicoes[idReposicao];
}

int Sistema::getNumUsuarios() const { return numUsuarios; }

int Sistema::getNumProdutos() const { return numProdutos; }

int Sistema::getNumCompras() const { return numCompras; }

int Sistema::getNumReposicoes() const { return numReposicoes; }

void Sistema::expandirUsuarios() {
  int novaCapacidade = capUsuarios * 2;
  if (novaCapacidade < 8) {
    novaCapacidade = 8;
  }

  Usuario *novosUsuarios = new Usuario[novaCapacidade];
  for (int i = 0; i < numUsuarios; i++) {
    novosUsuarios[i] = usuarios[i];
  }

  delete[] usuarios;
  usuarios = novosUsuarios;
  capUsuarios = novaCapacidade;
}

void Sistema::expandirProdutos() {
  int novaCapacidade = capProdutos * 2;
  if (novaCapacidade < 8) {
    novaCapacidade = 8;
  }

  Produto *novosProdutos = new Produto[novaCapacidade];
  for (int i = 0; i < numProdutos; i++) {
    novosProdutos[i] = produtos[i];
  }

  delete[] produtos;
  produtos = novosProdutos;
  capProdutos = novaCapacidade;
}

void Sistema::expandirCompras() {
  int novaCapacidade = capCompras * 2;
  if (novaCapacidade < 8) {
    novaCapacidade = 8;
  }

  Compra *novasCompras = new Compra[novaCapacidade];
  for (int i = 0; i < numCompras; i++) {
    novasCompras[i] = compras[i];
  }

  delete[] compras;
  compras = novasCompras;
  capCompras = novaCapacidade;
}

void Sistema::expandirReposicoes() {
  int novaCapacidade = capReposicoes * 2;
  if (novaCapacidade < 8) {
    novaCapacidade = 8;
  }

  Reposicao *novasReposicoes = new Reposicao[novaCapacidade];
  for (int i = 0; i < numReposicoes; i++) {
    novasReposicoes[i] = reposicoes[i];
  }

  delete[] reposicoes;
  reposicoes = novasReposicoes;
  capReposicoes = novaCapacidade;
}

void Sistema::ordenarItensPorProduto(ItemEstoque *itens, int numItens) const {
  // Insertion sort simples pq cada comando costuma ter poucos produtos
  for (int i = 1; i < numItens; i++) {
    ItemEstoque atual = itens[i];
    int j = i - 1;

    while (j >= 0 && itens[j].idProduto > atual.idProduto) {
      itens[j + 1] = itens[j];
      j--;
    }

    itens[j + 1] = atual;
  }
}

bool Sistema::validarCompra(const ItemEstoque *itens, int numItens) const {
  for (int i = 0; i < numItens; i++) {
    int idProduto = itens[i].idProduto;
    if (idProduto < 0 || idProduto >= numProdutos) {
      return false;
    }
    if (!produtos[idProduto].temEstoque(itens[i].quantidade)) {
      return false;
    }
  }

  return true;
}

void Sistema::aplicarCompra(int idUsuario, const ItemEstoque *itens,
                            int numItens) {
  for (int i = 0; i < numItens; i++) {
    int idProduto = itens[i].idProduto;
    int quantidade = itens[i].quantidade;

    produtos[idProduto].decrementarEstoque(quantidade);
    produtos[idProduto].registrarUsuarioComprador(idUsuario, quantidade);
    usuarios[idUsuario].registrarProdutoComprado(idProduto, quantidade);
  }
}

void Sistema::aplicarReposicao(const ItemEstoque *itens, int numItens) {
  for (int i = 0; i < numItens; i++) {
    produtos[itens[i].idProduto].incrementarEstoque(itens[i].quantidade);
  }
}

int Sistema::copiarIds(const ListaIds *lista, int *resultado) const {
  if (lista == 0) {
    return 0;
  }

  int quantidade = lista->getQuantidade();
  for (int i = 0; i < quantidade; i++) {
    resultado[i] = lista->getId(i);
  }

  return quantidade;
}

int Sistema::copiarId(int id, int *resultado) const {
  if (id < 0) {
    return 0;
  }

  resultado[0] = id;
  return 1;
}

int Sistema::intersecao(const int *a, int qtdA, const int *b, int qtdB,
                        int *resultado) const {
  // Mesmo principio do merge, mas guardando apenas os ids que aparecem nas duas listas (double pointer)
  int i = 0;
  int j = 0;
  int qtdResultado = 0;

  while (i < qtdA && j < qtdB) {
    if (a[i] == b[j]) {
      resultado[qtdResultado] = a[i];
      qtdResultado++;
      i++;
      j++;
    } else if (a[i] < b[j]) {
      i++;
    } else {
      j++;
    }
  }

  return qtdResultado;
}

int Sistema::filtrarUsuarios(const std::string &atributo,
                             const std::string &valor,
                             int *resultado) const {
  if (atributo == "id") {
    // id nao precisa passar pela hash, pois tambem e posicao no vetor principal (acesso constante natural)
    int idUsuario = std::atoi(valor.c_str());
    if (idUsuario >= 0 && idUsuario < numUsuarios) {
      return copiarId(idUsuario, resultado);
    }
    return 0;
  }
  if (atributo == "nome") {
    return copiarIds(indicesUsuarios.nome.buscar(valor), resultado);
  }
  if (atributo == "idade") {
    return copiarIds(indicesUsuarios.idade.buscar(valor), resultado);
  }
  if (atributo == "cidade") {
    return copiarIds(indicesUsuarios.cidade.buscar(valor), resultado);
  }
  if (atributo == "estado") {
    return copiarIds(indicesUsuarios.estado.buscar(valor), resultado);
  }
  if (atributo == "nacionalidade") {
    return copiarIds(indicesUsuarios.nacionalidade.buscar(valor), resultado);
  }

  return 0;
}

int Sistema::filtrarProdutos(const std::string &atributo,
                             const std::string &valor,
                             int *resultado) const {
  if (atributo == "id") {
    int idProduto = std::atoi(valor.c_str());
    if (idProduto >= 0 && idProduto < numProdutos) {
      return copiarId(idProduto, resultado);
    }
    return 0;
  }
  if (atributo == "nome") {
    return copiarIds(indicesProdutos.nome.buscar(valor), resultado);
  }
  if (atributo == "categoria") {
    return copiarIds(indicesProdutos.categoria.buscar(valor), resultado);
  }
  if (atributo == "marca") {
    return copiarIds(indicesProdutos.marca.buscar(valor), resultado);
  }
  if (atributo == "condicao") {
    return copiarIds(indicesProdutos.condicao.buscar(valor), resultado);
  }

  return 0;
}

int Sistema::filtrarCompras(const std::string &atributo,
                            const std::string &valor,
                            int *resultado) const {
  if (atributo == "id") {
    int idCompra = std::atoi(valor.c_str());
    if (idCompra >= 0 && idCompra < numCompras) {
      return copiarId(idCompra, resultado);
    }
    return 0;
  }
  if (atributo == "timestamp") {
    return copiarIds(indicesCompras.timestamp.buscar(valor), resultado);
  }
  if (atributo == "id_usuario") {
    return copiarIds(indicesCompras.usuario.buscar(valor), resultado);
  }
  if (atributo == "id_produto") {
    return copiarIds(indicesCompras.produto.buscar(valor), resultado);
  }

  return 0;
}

int Sistema::filtrarReposicoes(const std::string &atributo,
                               const std::string &valor,
                               int *resultado) const {
  if (atributo == "id") {
    int idReposicao = std::atoi(valor.c_str());
    if (idReposicao >= 0 && idReposicao < numReposicoes) {
      return copiarId(idReposicao, resultado);
    }
    return 0;
  }
  if (atributo == "timestamp") {
    return copiarIds(indicesReposicoes.timestamp.buscar(valor), resultado);
  }
  if (atributo == "id_produto") {
    return copiarIds(indicesReposicoes.produto.buscar(valor), resultado);
  }

  return 0;
}
