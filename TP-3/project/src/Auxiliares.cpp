#include "Auxiliares.h"

#include <sstream>

// Inteiros entram na hash como texto para reaproveitar o mesmo indice de strings
std::string chaveInteiro(int valor) {
  std::stringstream ss;
  ss << valor;
  return ss.str();
}

// Preco fica em centavos para imprimir exatamente as duas casas do enunciado
int precoParaCentavos(const std::string &preco) {
  int reais = 0;
  int centavos = 0;
  int i = 0;

  while (i < (int)preco.size() && preco[i] != '.') {
    reais = reais * 10 + (preco[i] - '0');
    i++;
  }

  if (i < (int)preco.size() && preco[i] == '.') {
    i++;
  }

  if (i < (int)preco.size()) {
    centavos += (preco[i] - '0') * 10;
    i++;
  }
  if (i < (int)preco.size()) {
    centavos += (preco[i] - '0');
  }

  return reais * 100 + centavos;
}

void imprimirPreco(std::ostream &os, int precoCentavos) {
  int reais = precoCentavos / 100;
  int centavos = precoCentavos % 100;

  os << reais << ".";
  if (centavos < 10) {
    os << "0";
  }
  os << centavos;
}
