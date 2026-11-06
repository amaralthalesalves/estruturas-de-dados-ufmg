#include "Sistema.h"
#include <cstdio>

//Inicializa o sistema com w (janela), N acoes, u clientes e as metricas ativas
void Sistema::inicializar(int w, int N, int u, int numMetricas, int* ids) {
    this->w = w;
    this->N = N;
    this->u = u;
    this->numMetricas = numMetricas;
    for (int i = 0; i < numMetricas; i++)
        metricaIds[i] = ids[i];

    //Aloca e inicializa as acoes com IDs de 0 a N-1
    acoes = new Acao[N];
    for (int i = 0; i < N; i++)
        acoes[i].inicializar(i, w);

    //Aloca e inicializa os clientes com IDs de 0 a u-1
    clientes = new Cliente[u];
    for (int i = 0; i < u; i++)
        clientes[i].inicializar(i, N);

#ifdef ESTRATEGIA_IMEDIATA
    //Aloca a estrutura da estrategia imediata:
    //rankingMetrica[k][posicao] guarda o ID da acao que esta naquela posicao do ranking
    rankingMetrica = new int*[numMetricas];
    for (int k = 0; k < numMetricas; k++) {
        rankingMetrica[k] = new int[N];
        for (int a = 0; a < N; a++)
            rankingMetrica[k][a] = a;    //ordem inicial por ID crescente (regra de desempate)
    }
#else
    //Na estrategia sob demanda nao usa esse array
    rankingMetrica = nullptr;
#endif

    //Arrays auxiliares reutilizaveis para as consultas (alocados uma vez so)
    posicaoNaMetrica = new int[numMetricas * N];
    ordMetrica = new AcaoMetrica[N];
    ordMetricaAux = new AcaoMetrica[N];
    ordPontuacao = new AcaoPontuacao[N];
    ordPontuacaoAux = new AcaoPontuacao[N];
}

//Libera toda a memoria alocada pelo sistema
void Sistema::destruir() {
    for (int i = 0; i < N; i++)
        acoes[i].destruir();
    delete[] acoes;

    for (int i = 0; i < u; i++)
        clientes[i].destruir();
    delete[] clientes;

    //Libera array da estrategia imediata (se foi alocado)
    if (rankingMetrica != nullptr) {
        for (int k = 0; k < numMetricas; k++)
            delete[] rankingMetrica[k];
        delete[] rankingMetrica;
    }

    //Libera arrays auxiliares das consultas
    delete[] posicaoNaMetrica;
    delete[] ordMetrica;
    delete[] ordMetricaAux;
    delete[] ordPontuacao;
    delete[] ordPontuacaoAux;
}

//Comando P - registra uma nova cotacao para a acao e atualiza os rankings se estrategia imediata
void Sistema::tratarCotacao(int acao, double preco) {
    acoes[acao].registrarCotacao(preco);

#ifdef ESTRATEGIA_IMEDIATA
    //Para cada metrica ativa, recalcula os valores de todas as acoes e reordena o ranking
    for (int k = 0; k < numMetricas; k++) {
        int idMetrica = metricaIds[k];

        //Preenche ordMetrica com o valor atualizado de cada acao
        for (int a = 0; a < N; a++) {
            ordMetrica[a].acao = a;
            ordMetrica[a].valor = acoes[a].calcularMetrica(idMetrica);
        }

        //Reordena com o mergesort ja implementado
        mergesortMetrica(ordMetrica, N, ordMetricaAux);

        //Atualiza o ranking com a nova ordem
        for (int i = 0; i < N; i++)
            rankingMetrica[k][i] = ordMetrica[i].acao;
    }
#endif
}

//Comando B - compra: adiciona a acao a carteira do cliente
void Sistema::tratarCompra(int cliente, int acao) {
    clientes[cliente].comprar(acao);
}

//Comando V - venda: remove a acao da carteira do cliente
void Sistema::tratarVenda(int cliente, int acao) {
    clientes[cliente].vender(acao);
}

//Comando Q - consulta: calcula e imprime as melhores e piores acoes da carteira do cliente
//Passo 1: monta as posicoes de cada acao no ranking de cada metrica da consulta
//Passo 2: calcula a pontuacao ponderada de cada acao na carteira
//Passo 3: ordena as acoes da carteira por pontuacao
//Passo 4: imprime as melhores e piores
void Sistema::tratarConsulta(int idConsulta, int cliente, int qtdAcoesDestaque,
                              int qtdMetricas, int* queryMetricaIds, double* pesos) {

    //Passo 1 - monta posicaoNaMetrica[j * N + acao] = posicao da acao no ranking da j-esima metrica
#ifdef ESTRATEGIA_IMEDIATA
    //Na estrategia imediata, os rankings ja estao prontos em rankingMetrica
    //Basta percorrer e anotar a posicao de cada acao
    for (int j = 0; j < qtdMetricas; j++) {
        int idMetrica = queryMetricaIds[j];

        //Encontra qual indice k corresponde a essa metrica nos arrays alocados
        int indiceMetrica = -1;
        for (int k = 0; k < numMetricas; k++) {
            if (metricaIds[k] == idMetrica) {
                indiceMetrica = k;
                break;
            }
        }

        //Percorre o ranking pronto e anota a posicao de cada acao
        for (int i = 0; i < N; i++) {
            int acaoNaPosicao = rankingMetrica[indiceMetrica][i];
            posicaoNaMetrica[j * N + acaoNaPosicao] = i;
        }
    }
#else
    //Na estrategia sob demanda, calcula o valor da metrica para todas as acoes e ordena com mergesort
    for (int j = 0; j < qtdMetricas; j++) {
        int idMetrica = queryMetricaIds[j];

        //Preenche o array com o valor da metrica de cada acao
        for (int a = 0; a < N; a++) {
            ordMetrica[a].acao = a;
            ordMetrica[a].valor = acoes[a].calcularMetrica(idMetrica);
        }

        //Ordena decrescente por valor, desempate crescente por ID
        mergesortMetrica(ordMetrica, N, ordMetricaAux);

        //Anota a posicao de cada acao no ranking resultante
        for (int i = 0; i < N; i++)
            posicaoNaMetrica[j * N + ordMetrica[i].acao] = i;
    }
#endif

    //Passo 2 - para cada acao na carteira do cliente, calcula a pontuacao ponderada
    //Pontuacao = soma de (N - posicao no ranking) * peso, para cada metrica
    int totalNaCarteira = 0;
    for (int a = 0; a < N; a++) {
        if (clientes[cliente].temAcao(a)) {
            double pontuacao = 0.0;
            for (int j = 0; j < qtdMetricas; j++) {
                int posicao = posicaoNaMetrica[j * N + a];
                pontuacao += (double)(N - posicao) * pesos[j];
            }
            ordPontuacao[totalNaCarteira].acao = a;
            ordPontuacao[totalNaCarteira].pontuacao = pontuacao;
            totalNaCarteira++;
        }
    }

    //Passo 3 - ordena as acoes da carteira por pontuacao decrescente, desempate por ID crescente
    mergesortPontuacao(ordPontuacao, totalNaCarteira, ordPontuacaoAux);

    //Passo 4 - imprime as melhores e piores acoes
    int qtdMelhores = qtdAcoesDestaque;
    if (qtdMelhores > totalNaCarteira) qtdMelhores = totalNaCarteira;
    int qtdPiores = qtdAcoesDestaque;
    if (qtdPiores > totalNaCarteira) qtdPiores = totalNaCarteira;

    //Melhores: do inicio do array ordenado (maior pontuacao primeiro)
    for (int i = 0; i < qtdMelhores; i++) {
        printf("R %d M %d %d %.2f\n", idConsulta, i,
               ordPontuacao[i].acao, ordPontuacao[i].pontuacao);
    }

    //Piores: do final do array ordenado (menor pontuacao primeiro)
    for (int i = 0; i < qtdPiores; i++) {
        int indicePior = totalNaCarteira - 1 - i;
        printf("R %d P %d %d %.2f\n", idConsulta, i,
               ordPontuacao[indicePior].acao, ordPontuacao[indicePior].pontuacao);
    }
}
