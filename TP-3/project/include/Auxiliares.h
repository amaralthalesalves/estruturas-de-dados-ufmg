#ifndef AUXILIARES_H
#define AUXILIARES_H

#include <ostream>
#include <string>

std::string chaveInteiro(int valor);
int precoParaCentavos(const std::string& preco);
void imprimirPreco(std::ostream& os, int precoCentavos);

#endif
