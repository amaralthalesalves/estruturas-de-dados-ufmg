#include "Recomendador.h"
#include "BuffersConsulta.h"
#include <iostream>

bool Recomendador::ordemRecomendacaoScore(const Recomendacao &a, const Recomendacao &b)
{
    double epsilon = 1e-9;
    double diff = a.score - b.score;
    if (diff > epsilon)
        return true;
    if (diff < -epsilon)
        return false;
    return a.idUsuario < b.idUsuario;
}

// Mergesort basico baseado no q esta em Ordenacao, mas para Recomendacao
void Recomendador::mergeSortRecomendacoes(Recomendacao *dados, int n, Recomendacao *aux)
{
    if (n <= 1)
    {
        return;
    }

    int meio = n / 2;
    Recomendador::mergeSortRecomendacoes(dados, meio, aux);
    Recomendador::mergeSortRecomendacoes(dados + meio, n - meio, aux);

    int i = 0;
    int j = meio;
    int k = 0;

    while (i < meio && j < n)
    {
        if (Recomendador::ordemRecomendacaoScore(dados[i], dados[j]))
        {
            aux[k] = dados[i];
            i++;
        }
        else
        {
            aux[k] = dados[j];
            j++;
        }
        k++;
    }

    while (i < meio)
    {
        aux[k] = dados[i];
        i++;
        k++;
    }

    while (j < n)
    {
        aux[k] = dados[j];
        j++;
        k++;
    }

    for (int x = 0; x < n; x++)
    {
        dados[x] = aux[x];
    }
}

void Recomendador::ordenarRecomendacoes(Recomendacao *dados, int n, Recomendacao *aux)
{
    if (dados == 0 || aux == 0 || n <= 1)
    {
        return;
    }

    Recomendador::mergeSortRecomendacoes(dados, n, aux);
}

void Recomendador::calcularDistanciasSociais(int indiceOrigemSocial, const Grafo &grafoSocial, BuffersConsulta &buffers)
{
    int numVertices = grafoSocial.getNumVertices();
    // Inicializa distancias como -1 (indisponivel), valor padrao 
    for (int i = 0; i < numVertices; i++)
    {
        buffers.distanciasSociais[i] = -1;
    }

    int inicioFila = 0;
    int fimFila = 0;

    buffers.distanciasSociais[indiceOrigemSocial] = 0;
    buffers.filaBuscaSocial[fimFila] = indiceOrigemSocial;
    fimFila++;

    // Fila usada para visitar os usuarios na ordem da busca
    while (inicioFila < fimFila)
    {
        int indiceAtual = buffers.filaBuscaSocial[inicioFila];
        inicioFila++;

        int qtdVizinhos = grafoSocial.obterVizinhos(indiceAtual, buffers.indicesInternos, buffers.capacidade);
        for (int i = 0; i < qtdVizinhos; i++)
        {
            int indiceVizinho = buffers.indicesInternos[i];
            if (buffers.distanciasSociais[indiceVizinho] == -1)
            {
                buffers.distanciasSociais[indiceVizinho] =
                    buffers.distanciasSociais[indiceAtual] + 1;
                buffers.filaBuscaSocial[fimFila] = indiceVizinho;
                fimFila++;
            }
        }
    }
}

int Recomendador::marcarTemasInteresse(int idUsuarioBase, const Dicionario &dicionario, const Grafo &grafoTemas, BuffersConsulta &buffers)
{
    int numTemas = dicionario.getNumTemas();
    for (int i = 0; i < numTemas; i++)
    {
        buffers.temasMarcados[i] = 0;
    }

    int indiceUsuarioTemas = dicionario.getIndUsuarioTemas(idUsuarioBase);
    int qtdIndicesInternos = grafoTemas.obterVizinhos(indiceUsuarioTemas, buffers.indicesInternos, buffers.capacidade);

    int qtdTemasUsuarioBase = 0;
    for (int i = 0; i < qtdIndicesInternos; i++)
    {
        No vertice = grafoTemas.getVertice(buffers.indicesInternos[i]);
        if (vertice.tipo == TEMA && buffers.temasMarcados[vertice.idExterno] == 0)
        {
            buffers.temasMarcados[vertice.idExterno] = 1;
            qtdTemasUsuarioBase++;
        }
    }

    return qtdTemasUsuarioBase;
}

double Recomendador::calcularAfinidadeTematica(int idCandidato, int qtdTemasUsuarioBase, const Dicionario &dicionario, const Grafo &grafoTemas, BuffersConsulta &buffers)
{
    int indiceCandidatoTemas = dicionario.getIndUsuarioTemas(idCandidato);
    int qtdIndicesInternos = grafoTemas.obterVizinhos(indiceCandidatoTemas, buffers.indicesInternos, buffers.capacidade);
    int qtdTemasCandidato = 0;
    int intersecao = 0;

    for (int i = 0; i < qtdIndicesInternos; i++)
    {
        No vertice = grafoTemas.getVertice(buffers.indicesInternos[i]);
        if (vertice.tipo != TEMA)
        {
            continue;
        }

        int idTema = vertice.idExterno;
        qtdTemasCandidato++;

        if (buffers.temasMarcados[idTema] == 1)
        {
            intersecao++;
        }
    }

    // Formula definida no enunciado pra calcular afinidade tematica
    int uniao = qtdTemasUsuarioBase + qtdTemasCandidato - intersecao;
    if (uniao == 0)
    {
        return 0.0;
    }

    return (double)intersecao / (double)uniao;
}

void Recomendador::comandoP(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial, Grafo &grafoTemas, BuffersConsulta &buffers)
{
    int idUsuarioBase;
    int topk;
    double pesoProx;
    double pesoAfin;
    ss >> idUsuarioBase >> topk >> pesoProx >> pesoAfin;

    buffers.garantirCapacidade(grafoTemas.getNumVertices());

    int indiceUsuarioBaseSocial = dicionario.getIndUsuarioSocial(idUsuarioBase);
    Recomendador::calcularDistanciasSociais(indiceUsuarioBaseSocial, grafoSocial, buffers);

    int qtdTemasUsuarioBase = Recomendador::marcarTemasInteresse(idUsuarioBase, dicionario, grafoTemas, buffers);
    int qtdRecomendacoes = 0;
    int numUsuarios = dicionario.getNumUsuarios();

    // Testa cada usuario como possivel recomendacao
    for (int idCandidato = 0; idCandidato < numUsuarios; idCandidato++)
    {
        if (idCandidato == idUsuarioBase)
        {
            continue;
        }

        int indiceCandidatoSocial = dicionario.getIndUsuarioSocial(idCandidato);
        // Nao recomenda quem o usuario base ja segue
        if (grafoSocial.existeAresta(indiceUsuarioBaseSocial, indiceCandidatoSocial))
        {
            continue;
        }

        int distancia = buffers.distanciasSociais[indiceCandidatoSocial];
        double proximidade = distancia > 0 ? 2.0 / (double)distancia : 0.0;
        double afinidade = Recomendador::calcularAfinidadeTematica(
            idCandidato,
            qtdTemasUsuarioBase,
            dicionario,
            grafoTemas,
            buffers);

        buffers.recomendacoes[qtdRecomendacoes].idUsuario = idCandidato;
        // Formula de recomendacao definida no enunciado
        buffers.recomendacoes[qtdRecomendacoes].score =
            pesoProx * proximidade + pesoAfin * afinidade;
        qtdRecomendacoes++;
    }

    Recomendador::ordenarRecomendacoes(buffers.recomendacoes, qtdRecomendacoes, buffers.auxRecomendacoes);
    // Seleciona os topk recomendados, topk vindo da entrada
    int limite = topk < qtdRecomendacoes ? topk : qtdRecomendacoes;

    std::cout << "P " << dicionario.getUsuario(idUsuarioBase).getNome();
    for (int i = 0; i < limite; i++)
    {
        int idRecomendado = buffers.recomendacoes[i].idUsuario;
        std::cout << " " << dicionario.getUsuario(idRecomendado).getNome();
    }
    std::cout << "\n";
}
