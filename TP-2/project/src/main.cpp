#include "Dicionario.h"
#include "Grafo.h"
#include "Ordenacao.h"
#include "Recomendador.h"
#include "BuffersConsulta.h"

#include <iostream>
#include <sstream>
#include <string>

//Main.cpp - parsing de entradas

// Transforma o char da entrada do comando A em um modo de armazenamento
static ModoArmazenamento modoArmazenamentoPorChar(char modo)
{
    if (modo == 'M')
    {
        return MATRIZ_ADJ;
    }
    if (modo == 'E')
    {
        return MATRIZ_ESPARSA;
    }
    return LISTA_ADJ;
}

// Impressao padrao das listagens: prefixo e nome base, depois itens com espaco antes
static void imprimirTemas(const char *comando, const Usuario &usuario, int *idsTemas, int qtdTemas, const Dicionario &dicionario)
{
    std::cout << comando << " " << usuario.getNome();
    for (int i = 0; i < qtdTemas; i++)
    {
        std::cout << " " << dicionario.getTema(idsTemas[i]).getNome();
    }
    std::cout << "\n";
}

static void imprimirUsuarios(const char *comando, const Usuario &usuario, int *idsUsuarios, int qtdUsuarios, const Dicionario &dicionario)
{
    std::cout << comando << " " << usuario.getNome();
    for (int i = 0; i < qtdUsuarios; i++)
    {
        std::cout << " " << dicionario.getUsuario(idsUsuarios[i]).getNome();
    }
    std::cout << "\n";
}

// Comando A - muda o modo de armazenamento dos grafos
static void comandoA(std::stringstream &ss, Grafo &grafoSocial, Grafo &grafoTemas)
{
    char modoChar;
    ss >> modoChar;

    ModoArmazenamento novoModo = modoArmazenamentoPorChar(modoChar);
    grafoSocial.migrar(novoModo);
    grafoTemas.migrar(novoModo);

    std::cout << "A " << modoChar << "\n";
}

// Comando T - adiciona um tema ao grafo de temas
static void comandoT(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoTemas)
{
    std::string nome;
    char tipo;
    ss >> nome >> tipo;

    int idTema = dicionario.getNumTemas();
    Tema tema(idTema, nome, tipo);
    int indiceTemaNoGrafoTemas = grafoTemas.adicionarVertice(idTema, TEMA);
    int idTemaRegistrado = dicionario.registrarTema(tema, indiceTemaNoGrafoTemas);

    std::cout << "T " << idTemaRegistrado << "\n";
}

// Comando U - adiciona um usuario ao sistema
static void comandoU(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial, Grafo &grafoTemas)
{
    std::string nome;
    int idade;
    ss >> nome >> idade;

    int idUsuario = dicionario.getNumUsuarios();
    Usuario usuario(idUsuario, nome, idade);
    int indiceUsuarioNoGrafoSocial = grafoSocial.adicionarVertice(idUsuario, USUARIO);
    int indiceUsuarioNoGrafoTemas = grafoTemas.adicionarVertice(idUsuario, USUARIO);
    int idUsuarioRegistrado = dicionario.registrarUsuario(usuario, indiceUsuarioNoGrafoSocial, indiceUsuarioNoGrafoTemas);

    int idTema;
    while (ss >> idTema)
    {
        int indiceTemaNoGrafoTemas = dicionario.getIndTemaTemas(idTema);
        grafoTemas.inserirAresta(indiceUsuarioNoGrafoTemas, indiceTemaNoGrafoTemas);
    }

    std::cout << "U " << idUsuarioRegistrado << "\n";
}

// Comando S - adiciona uma aresta entre dois usuarios no grafo social (relacao de seguimento)
static void comandoS(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial)
{
    int idOrigem;
    int idDestino;
    ss >> idOrigem >> idDestino;

    int origem = dicionario.getIndUsuarioSocial(idOrigem);
    int destino = dicionario.getIndUsuarioSocial(idDestino);
    grafoSocial.inserirAresta(origem, destino);

    std::cout << "S " << dicionario.getUsuario(idOrigem).getNome()
              << " " << dicionario.getUsuario(idDestino).getNome() << "\n";
}

// Comando R - remove uma aresta entre dois usuarios no grafo social (deixar de seguir)
static void comandoR(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial)
{
    int idOrigem;
    int idDestino;
    ss >> idOrigem >> idDestino;

    int origem = dicionario.getIndUsuarioSocial(idOrigem);
    int destino = dicionario.getIndUsuarioSocial(idDestino);
    grafoSocial.removerAresta(origem, destino);

    std::cout << "R " << dicionario.getUsuario(idOrigem).getNome()
              << " " << dicionario.getUsuario(idDestino).getNome() << "\n";
}

// Comando LT - lista os temas de interesse do usuario em ordem crescente de ID
static void comandoLT(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoTemas, Ordenacao &ordenacao, BuffersConsulta &buffers)
{
    int idUsuario;
    ss >> idUsuario;

    buffers.garantirCapacidade(grafoTemas.getNumVertices());

    // Busca os vizinhos do usuario no grafo de temas, que representam seus interesses
    int indiceUsuarioTemas = dicionario.getIndUsuarioTemas(idUsuario);
    int qtdIndicesInternos = grafoTemas.obterVizinhos(indiceUsuarioTemas, buffers.indicesInternos, buffers.capacidade);

    // Filtra apenas vertices do tipo tema e guarda seus IDs externos para ordenacao
    int qtdTemas = 0;
    for (int i = 0; i < qtdIndicesInternos; i++)
    {
        No vertice = grafoTemas.getVertice(buffers.indicesInternos[i]);
        if (vertice.tipo == TEMA)
        {
            buffers.idsExternos[qtdTemas] = vertice.idExterno;
            qtdTemas++;
        }
    }

    // Ordena por ID antes de imprimir, mantendo o formato exigido pela consulta
    ordenacao.mergeSortIds(buffers.idsExternos, qtdTemas, buffers.auxOrdenacao);
    imprimirTemas("LT", dicionario.getUsuario(idUsuario), buffers.idsExternos, qtdTemas, dicionario);
}

// Comando LC - lista os usuarios que seguem o usuario consultado
static void comandoLC(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial, Ordenacao &ordenacao, BuffersConsulta &buffers)
{
    int idUsuario;
    ss >> idUsuario;

    buffers.garantirCapacidade(grafoSocial.getNumVertices());

    // Seguidores sao predecessores no grafo social direcionado
    int indiceUsuarioSocial = dicionario.getIndUsuarioSocial(idUsuario);
    int qtdIndicesInternos = grafoSocial.obterPredecessores(indiceUsuarioSocial, buffers.indicesInternos, buffers.capacidade);

    // Converte os indices internos do grafo para IDs externos de usuarios
    for (int i = 0; i < qtdIndicesInternos; i++)
    {
        buffers.idsExternos[i] = grafoSocial.getVertice(buffers.indicesInternos[i]).idExterno;
    }

    // Ordena os seguidores por ID antes de imprimir
    ordenacao.mergeSortIds(buffers.idsExternos, qtdIndicesInternos, buffers.auxOrdenacao);
    imprimirUsuarios("LC", dicionario.getUsuario(idUsuario), buffers.idsExternos, qtdIndicesInternos, dicionario);
}

// Comando LS - lista os usuarios seguidos pelo usuario consultado
static void comandoLS(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial, Ordenacao &ordenacao, BuffersConsulta &buffers)
{
    int idUsuario;
    ss >> idUsuario;

    buffers.garantirCapacidade(grafoSocial.getNumVertices());

    // Seguidos sao vizinhos de saida no grafo social direcionado
    int indiceUsuarioSocial = dicionario.getIndUsuarioSocial(idUsuario);
    int qtdIndicesInternos = grafoSocial.obterVizinhos(indiceUsuarioSocial, buffers.indicesInternos, buffers.capacidade);

    // Converte os indices internos do grafo para IDs externos de usuarios
    for (int i = 0; i < qtdIndicesInternos; i++)
    {
        buffers.idsExternos[i] = grafoSocial.getVertice(buffers.indicesInternos[i]).idExterno;
    }

    // Ordena os seguidos por ID antes de imprimir
    ordenacao.mergeSortIds(buffers.idsExternos, qtdIndicesInternos, buffers.auxOrdenacao);
    imprimirUsuarios("LS", dicionario.getUsuario(idUsuario), buffers.idsExternos, qtdIndicesInternos, dicionario);
}

// Comando LA - lista amigos, ou seja, usuarios com relacao nos dois sentidos
static void comandoLA(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial, Ordenacao &ordenacao, BuffersConsulta &buffers)
{
    int idUsuario;
    ss >> idUsuario;

    buffers.garantirCapacidade(grafoSocial.getNumVertices());

    // Amigos precisam estar entre os usuarios seguidos diretamente
    int indiceUsuarioSocial = dicionario.getIndUsuarioSocial(idUsuario);
    int qtdSeguidos = grafoSocial.obterVizinhos(indiceUsuarioSocial, buffers.indicesInternos, buffers.capacidade);

    // Mantem apenas os seguidos que tambem seguem o usuario de volta
    int qtdAmigos = 0;
    for (int i = 0; i < qtdSeguidos; i++)
    {
        int indiceSeguido = buffers.indicesInternos[i];
        if (grafoSocial.existeAresta(indiceSeguido, indiceUsuarioSocial))
        {
            buffers.idsExternos[qtdAmigos] = grafoSocial.getVertice(indiceSeguido).idExterno;
            qtdAmigos++;
        }
    }

    // Ordena os amigos por ID antes de imprimir
    ordenacao.mergeSortIds(buffers.idsExternos, qtdAmigos, buffers.auxOrdenacao);
    imprimirUsuarios("LA", dicionario.getUsuario(idUsuario), buffers.idsExternos, qtdAmigos, dicionario);
}

// Comando Q - consulta a relacao entre dois usuarios no grafo social
static void comandoQ(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoSocial)
{
    int idUsuario1;
    int idUsuario2;
    ss >> idUsuario1 >> idUsuario2;

    int indiceUsuario1 = dicionario.getIndUsuarioSocial(idUsuario1);
    int indiceUsuario2 = dicionario.getIndUsuarioSocial(idUsuario2);

    bool usuario1SegueUsuario2 = grafoSocial.existeAresta(indiceUsuario1, indiceUsuario2);
    bool usuario2SegueUsuario1 = grafoSocial.existeAresta(indiceUsuario2, indiceUsuario1);

    // Codificacao da resposta: 0 sem relacao, 1 usuario1 segue usuario2, 2 usuario2 segue usuario1, 3 ambos
    int valor = 0;
    if (usuario1SegueUsuario2 && usuario2SegueUsuario1)
    {
        valor = 3;
    }
    else if (usuario1SegueUsuario2)
    {
        valor = 1;
    }
    else if (usuario2SegueUsuario1)
    {
        valor = 2;
    }

    std::cout << "Q " << dicionario.getUsuario(idUsuario1).getNome()
              << " " << dicionario.getUsuario(idUsuario2).getNome()
              << " " << valor << "\n";
}

// Comando G - verifica se um usuario possui interesse em um tema
static void comandoG(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoTemas)
{
    int idUsuario;
    int idTema;
    ss >> idUsuario >> idTema;

    // No grafo de temas, a aresta usuario-tema representa interesse
    int indiceUsuarioTemas = dicionario.getIndUsuarioTemas(idUsuario);
    int indiceTema = dicionario.getIndTemaTemas(idTema);
    int possuiInteresse = grafoTemas.existeAresta(indiceUsuarioTemas, indiceTema) ? 1 : 0;

    std::cout << "G " << dicionario.getUsuario(idUsuario).getNome()
              << " " << dicionario.getTema(idTema).getNome()
              << " " << possuiInteresse << "\n";
}

// Comando F - calcula a popularidade de um tema pelo numero de usuarios interessados
static void comandoF(std::stringstream &ss, Dicionario &dicionario, Grafo &grafoTemas, BuffersConsulta &buffers)
{
    int idTema;
    ss >> idTema;

    buffers.garantirCapacidade(grafoTemas.getNumVertices());

    int indiceTema = dicionario.getIndTemaTemas(idTema);
    int qtdIndicesInternos = grafoTemas.obterVizinhos(indiceTema, buffers.indicesInternos, buffers.capacidade);

    // Como o grafo de temas tambem possui vertices de temas, conta apenas vizinhos que sao usuarios
    int popularidade = 0;
    for (int i = 0; i < qtdIndicesInternos; i++)
    {
        if (grafoTemas.getVertice(buffers.indicesInternos[i]).tipo == USUARIO)
        {
            popularidade++;
        }
    }

    std::cout << "F " << dicionario.getTema(idTema).getNome() << " " << popularidade << "\n";
}

int main()
{
    Dicionario dicionario;
    Grafo grafoSocial(true, LISTA_ADJ);
    Grafo grafoTemas(false, LISTA_ADJ);
    Ordenacao ordenacao;
    BuffersConsulta buffers;

    std::string linha;
    while (std::getline(std::cin, linha))
    {
        std::stringstream ss(linha);
        std::string comando;
        ss >> comando;
        
        if (comando == "A")
        {
            comandoA(ss, grafoSocial, grafoTemas);
        }
        else if (comando == "T")
        {
            comandoT(ss, dicionario, grafoTemas);
        }
        else if (comando == "U")
        {
            comandoU(ss, dicionario, grafoSocial, grafoTemas);
        }
        else if (comando == "S")
        {
            comandoS(ss, dicionario, grafoSocial);
        }
        else if (comando == "R")
        {
            comandoR(ss, dicionario, grafoSocial);
        }
        else if (comando == "LT")
        {
            comandoLT(ss, dicionario, grafoTemas, ordenacao, buffers);
        }
        else if (comando == "LC")
        {
            comandoLC(ss, dicionario, grafoSocial, ordenacao, buffers);
        }
        else if (comando == "LS")
        {
            comandoLS(ss, dicionario, grafoSocial, ordenacao, buffers);
        }
        else if (comando == "LA")
        {
            comandoLA(ss, dicionario, grafoSocial, ordenacao, buffers);
        }
        else if (comando == "Q")
        {
            comandoQ(ss, dicionario, grafoSocial);
        }
        else if (comando == "G")
        {
            comandoG(ss, dicionario, grafoTemas);
        }
        else if (comando == "F")
        {
            comandoF(ss, dicionario, grafoTemas, buffers);
        }
        else if (comando == "P")
        {
            Recomendador::comandoP(ss, dicionario, grafoSocial, grafoTemas, buffers);
        }
    }

    return 0;
}
