#include "Auxiliares.h"
#include "BuffersConsulta.h"
#include "Sistema.h"

#include <iostream>
#include <sstream>
#include <string>

static int lerItens(std::stringstream &ss, BuffersConsulta &buffers) {
  int numItens = 0;
  int idProduto;
  int quantidade;

  while (ss >> idProduto >> quantidade) {
    if (numItens == buffers.capacidadeItens) {
      buffers.garantirCapacidadeItens(numItens + 1);
    }

    buffers.itensTemporarios[numItens].idProduto = idProduto;
    buffers.itensTemporarios[numItens].quantidade = quantidade;
    numItens++;
  }

  return numItens;
}

static int lerFiltros(std::stringstream &ss, std::string *atributos,
                      std::string *valores) {
  int numFiltros = 0;

  while (ss >> atributos[numFiltros] >> valores[numFiltros]) {
    numFiltros++;
  }

  return numFiltros;
}

static void imprimirItens(const ItemEstoque *itens, int numItens) {
  for (int i = 0; i < numItens; i++) {
    if (i > 0) {
      std::cout << " ";
    }
    std::cout << "produto_" << i + 1 << " " << itens[i].idProduto << " "
              << itens[i].quantidade;
  }
  std::cout << "\n";
}

static void imprimirProdutosComprados(const ProdutoComprado *produtos,
                                      int numProdutos) {
  for (int i = 0; i < numProdutos; i++) {
    if (i > 0) {
      std::cout << " ";
    }
    std::cout << "produto_" << i + 1 << " " << produtos[i].idProduto << " "
              << produtos[i].qtdComprada;
  }
  std::cout << "\n";
}

static void imprimirUsuariosCompradores(const UsuarioComprador *usuarios,
                                        int numUsuarios) {
  for (int i = 0; i < numUsuarios; i++) {
    if (i > 0) {
      std::cout << " ";
    }
    std::cout << "usuario_" << i + 1 << " " << usuarios[i].idUsuario << " "
              << usuarios[i].qtdComprada;
  }
  std::cout << "\n";
}

// Comando U - cadastra usuario
static void comandoU(std::stringstream &ss, Sistema &sistema) {
  std::string nome;
  int idade;
  std::string cidade;
  std::string estado;
  std::string nacionalidade;

  ss >> nome >> idade >> cidade >> estado >> nacionalidade;

  int idUsuario =
      sistema.cadastrarUsuario(nome, idade, cidade, estado, nacionalidade);
  std::cout << "U " << idUsuario << "\n";
}

// Comando P - cadastra produto
static void comandoP(std::stringstream &ss, Sistema &sistema) {
  std::string nome;
  std::string preco;
  int qtd;
  std::string categoria;
  std::string marca;
  std::string condicao;

  ss >> nome >> preco >> qtd >> categoria >> marca >> condicao;

  int idProduto = sistema.cadastrarProduto(nome, precoParaCentavos(preco), qtd,
                                           categoria, marca, condicao);
  std::cout << "P " << idProduto << "\n";
}

// Comando R - registra entrada de estoque
static void comandoR(std::stringstream &ss, Sistema &sistema,
                     BuffersConsulta &buffers) {
  int timestamp;
  ss >> timestamp;

  int numItens = lerItens(ss, buffers);
  int idReposicao =
      sistema.registrarReposicao(timestamp, buffers.itensTemporarios, numItens);

  std::cout << "R " << idReposicao << "\n";
}

// Comando C - registra compra quando ha estoque suficiente
static void comandoC(std::stringstream &ss, Sistema &sistema,
                     BuffersConsulta &buffers) {
  int timestamp;
  int idUsuario;
  ss >> timestamp >> idUsuario;

  int numItens = lerItens(ss, buffers);
  int idCompra = sistema.registrarCompra(timestamp, idUsuario,
                                         buffers.itensTemporarios, numItens);

  if (idCompra == -1) {
    std::cout << "C INV\n";
  } else {
    std::cout << "C " << idCompra << "\n";
  }
}

// Consulta LU - usuarios filtrados por atributos
static void comandoLU(std::stringstream &ss, Sistema &sistema,
                      BuffersConsulta &buffers) {
  std::string atributos[8];
  std::string valores[8];
  int numFiltros = lerFiltros(ss, atributos, valores);
  int qtdResultados =
      sistema.consultarUsuarios(atributos, valores, numFiltros, buffers);

  if (qtdResultados == 0) {
    std::cout << "LU VAZIO\n";
    return;
  }

  for (int i = 0; i < qtdResultados; i++) {
    const Usuario &usuario = sistema.getUsuario(buffers.idsResultado[i]);
    std::cout << "LU resultado_" << i + 1 << " usuario " << usuario.getId()
              << " " << usuario.getNome() << " " << usuario.getIdade() << " "
              << usuario.getCidade() << " " << usuario.getEstado() << " "
              << usuario.getNacionalidade() << "\n";

    if (usuario.getNumProdutosComprados() > 0) {
      imprimirProdutosComprados(usuario.getProdutosComprados(),
                                usuario.getNumProdutosComprados());
    }
  }
}

// Consulta LP - produtos filtrados por atributos
static void comandoLP(std::stringstream &ss, Sistema &sistema,
                      BuffersConsulta &buffers) {
  std::string atributos[8];
  std::string valores[8];
  int numFiltros = lerFiltros(ss, atributos, valores);
  int qtdResultados =
      sistema.consultarProdutos(atributos, valores, numFiltros, buffers);

  if (qtdResultados == 0) {
    std::cout << "LP VAZIO\n";
    return;
  }

  for (int i = 0; i < qtdResultados; i++) {
    const Produto &produto = sistema.getProduto(buffers.idsResultado[i]);
    std::cout << "LP resultado_" << i + 1 << " produto " << produto.getId()
              << " " << produto.getNome() << " ";
    imprimirPreco(std::cout, produto.getPrecoCentavos());
    std::cout << " " << produto.getQtd() << " " << produto.getCategoria()
              << " " << produto.getMarca() << " " << produto.getCondicao()
              << "\n";

    if (produto.getNumUsuariosCompradores() > 0) {
      imprimirUsuariosCompradores(produto.getUsuariosCompradores(),
                                  produto.getNumUsuariosCompradores());
    }
  }
}

// Consulta LC - compras validas ja registradas
static void comandoLC(std::stringstream &ss, Sistema &sistema,
                      BuffersConsulta &buffers) {
  std::string atributos[8];
  std::string valores[8];
  int numFiltros = lerFiltros(ss, atributos, valores);
  int qtdResultados =
      sistema.consultarCompras(atributos, valores, numFiltros, buffers);

  if (qtdResultados == 0) {
    std::cout << "LC VAZIO\n";
    return;
  }

  for (int i = 0; i < qtdResultados; i++) {
    const Compra &compra = sistema.getCompra(buffers.idsResultado[i]);
    std::cout << "LC resultado_" << i + 1 << " compra " << compra.getId()
              << " timestamp " << compra.getTimestamp() << " usuario "
              << compra.getIdUsuario() << "\n";
    imprimirItens(compra.getItens(), compra.getNumItens());
  }
}

// Consulta LR - reposicoes registradas
static void comandoLR(std::stringstream &ss, Sistema &sistema,
                      BuffersConsulta &buffers) {
  std::string atributos[8];
  std::string valores[8];
  int numFiltros = lerFiltros(ss, atributos, valores);
  int qtdResultados =
      sistema.consultarReposicoes(atributos, valores, numFiltros, buffers);

  if (qtdResultados == 0) {
    std::cout << "LR VAZIO\n";
    return;
  }

  for (int i = 0; i < qtdResultados; i++) {
    const Reposicao &reposicao = sistema.getReposicao(buffers.idsResultado[i]);
    std::cout << "LR resultado_" << i + 1 << " reposicao "
              << reposicao.getId() << " timestamp " << reposicao.getTimestamp()
              << "\n";
    imprimirItens(reposicao.getItens(), reposicao.getNumItens());
  }
}

int main() {
  Sistema sistema;
  BuffersConsulta buffers;

  std::string linha;
  while (std::getline(std::cin, linha)) {
    std::stringstream ss(linha);
    std::string comando;
    ss >> comando;

    if (comando == "U") {
      comandoU(ss, sistema);
    } else if (comando == "P") {
      comandoP(ss, sistema);
    } else if (comando == "R") {
      comandoR(ss, sistema, buffers);
    } else if (comando == "C") {
      comandoC(ss, sistema, buffers);
    } else if (comando == "LU") {
      comandoLU(ss, sistema, buffers);
    } else if (comando == "LP") {
      comandoLP(ss, sistema, buffers);
    } else if (comando == "LC") {
      comandoLC(ss, sistema, buffers);
    } else if (comando == "LR") {
      comandoLR(ss, sistema, buffers);
    }
  }

  return 0;
}
