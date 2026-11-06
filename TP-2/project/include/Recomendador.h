#ifndef RECOMENDADOR_H
#define RECOMENDADOR_H

#include "Dicionario.h"
#include "Grafo.h"
#include <sstream>
//TAD Recomendador - responsavel por gerar recomendacoes de usuarios com base em proximidade social e afinidade tematica (ponto extra)

// Dados de uma recomendacao do comando P
struct Recomendacao
{
    int idUsuario;
    double score;
};

// Declaracao antecipada dos buffers pra poder usar nas funcoes
struct BuffersConsulta;

class Recomendador
{
private:
    // Compara recomendacoes pelo score e depois pelo id do usuario
    // Usar epsilon para comparar doubles, ja q a subtracao pode gerar imprecisoes
    static bool ordemRecomendacaoScore(const Recomendacao &a, const Recomendacao &b);

    // Ordena as recomendacoes na ordem de impressao
    static void mergeSortRecomendacoes(Recomendacao *dados, int n, Recomendacao *aux);

    static void ordenarRecomendacoes(Recomendacao *dados, int n, Recomendacao *aux);

    // Calcula as distancias sociais a partir de um usuario
    static void calcularDistanciasSociais(int indiceOrigemSocial, const Grafo &grafoSocial, BuffersConsulta &buffers);

    // Marca os temas de interesse do usuario base
    static int marcarTemasInteresse(int idUsuarioBase, const Dicionario &dicionario, const Grafo &grafoTemas, BuffersConsulta &buffers);

    // Calcula a afinidade tematica entre o usuario base e um candidato
    static double calcularAfinidadeTematica(int idCandidato, int qtdTemasUsuarioBase, const Dicionario &dicionario, const Grafo &grafoTemas, BuffersConsulta &buffers);

public:
    // Comando P - recomenda usuarios usando proximidade social e afinidade tematica
    static void comandoP(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial, Grafo &grafoTemas, BuffersConsulta &buffers);
};

#endif
