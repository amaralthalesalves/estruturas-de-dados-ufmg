#include "Sistema.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

// Converte nome de metrica (string) para indice global fixo
// RET=0, AVGRET=1, STAB=2, CONS=3
int converterMetrica(const char* nome) {
    if (strcmp(nome, "RET") == 0)    return 0;
    if (strcmp(nome, "AVGRET") == 0) return 1;
    if (strcmp(nome, "STAB") == 0)   return 2;
    if (strcmp(nome, "CONS") == 0)   return 3;
    return -1;
}

int main() {
    //Linha M: ler w e metricas ativas
    int w, numMetricas = 0;
    int metricaIds[4];
    char linha[1024];
    fgets(linha, sizeof(linha), stdin);

    //Extrai w e os nomes das metricas da linha M
    char* token = strtok(linha, " \r\n");  // "M"
    token = strtok(NULL, " \r\n");         // w
    w = atoi(token);
    while ((token = strtok(NULL, " \r\n")) != NULL)
        metricaIds[numMetricas++] = converterMetrica(token);

    //Lê todas as linhas A e U apenas contando para saber N e u
    //antes de inicializar o sistema. (Assumimos IDs em sequencia 0..N-1 e 0..u-1)
    int N = 0, u = 0;
    char cmd;
    int id;
    bool lendoDeclaracoes = true;
    while (lendoDeclaracoes && scanf(" %c", &cmd) == 1) {
        if (cmd == 'A') {
            scanf("%d", &id);
            N++;
        } else if (cmd == 'U') {
            scanf("%d", &id);
            u++;
        } else {
            //Primeiro comando P/B/V/Q — para de contar
            lendoDeclaracoes = false;
        }
    }

    //Inicializa o sistema com N acoes, u clientes e as metricas ativas
    Sistema sistema;
    sistema.inicializar(w, N, u, numMetricas, metricaIds);

    //Processa o primeiro comando que ja foi lido, e os restantes
    bool temComando = true;
    while (temComando) {
        if (cmd == 'P') {
            int acao;
            double preco;
            scanf("%d %lf", &acao, &preco);
            sistema.tratarCotacao(acao, preco);

        } else if (cmd == 'B') {
            int cliente, acao;
            scanf("%d %d", &cliente, &acao);
            sistema.tratarCompra(cliente, acao);

        } else if (cmd == 'V') {
            int cliente, acao;
            scanf("%d %d", &cliente, &acao);
            sistema.tratarVenda(cliente, acao);

        } else if (cmd == 'Q') {
            int idConsulta, cliente, qtdAcoesDestaque, qtdMetricas;
            scanf("%d %d %d %d", &idConsulta, &cliente,
                  &qtdAcoesDestaque, &qtdMetricas);
            int queryMetricaIds[4];
            double pesos[4];
            for (int i = 0; i < qtdMetricas; i++) {
                char nomeMetrica[16];
                scanf(" %15s %lf", nomeMetrica, &pesos[i]);
                queryMetricaIds[i] = converterMetrica(nomeMetrica);
            }
            sistema.tratarConsulta(idConsulta, cliente, qtdAcoesDestaque,
                                   qtdMetricas, queryMetricaIds, pesos);
        }

        temComando = (scanf(" %c", &cmd) == 1);
    }

    sistema.destruir();
    return 0;
}