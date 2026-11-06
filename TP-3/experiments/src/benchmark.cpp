#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

struct Registro {
  int valor[4];
};

struct Filtro {
  int atributo;
  int valor;
};

static volatile long long somaGlobal = 0;

Registro gerarRegistro(int id, int escala, int semente) {
  Registro r;
  int c0 = escala / 25 + 5;
  int c1 = escala / 40 + 7;
  int c2 = 16;
  int c3 = escala / 30 + 9;

  r.valor[0] = (id * 37 + semente) % c0;
  r.valor[1] = (id * 17 + semente * 3) % c1;
  r.valor[2] = (id * 11 + semente) % c2;
  r.valor[3] = (id * 53 + semente * 5) % c3;

  return r;
}

void gerarFiltros(Filtro *filtros, int qtdFiltros, int consulta, int escala,
                  int semente) {
  Registro r = gerarRegistro((consulta * 13 + semente) % escala, escala, semente);

  for (int i = 0; i < qtdFiltros; i++) {
    int atributo = i % 4;
    filtros[i].atributo = atributo;
    filtros[i].valor = r.valor[atributo];
  }
}

double agoraMs() {
  using namespace std::chrono;
  return duration<double, std::milli>(high_resolution_clock::now().time_since_epoch())
      .count();
}

double mediana(double *valores, int qtd) {
  for (int i = 1; i < qtd; i++) {
    double atual = valores[i];
    int j = i - 1;
    while (j >= 0 && valores[j] > atual) {
      valores[j + 1] = valores[j];
      j--;
    }
    valores[j + 1] = atual;
  }

  return valores[qtd / 2];
}

class ListaVetor {
private:
  int *dados;
  int quantidade;
  int capacidade;

  void expandir() {
    int novaCapacidade = capacidade * 2;
    if (novaCapacidade < 4) {
      novaCapacidade = 4;
    }

    int *novosDados = new int[novaCapacidade];
    for (int i = 0; i < quantidade; i++) {
      novosDados[i] = dados[i];
    }

    delete[] dados;
    dados = novosDados;
    capacidade = novaCapacidade;
  }

  ListaVetor(const ListaVetor &);
  ListaVetor &operator=(const ListaVetor &);

public:
  ListaVetor() : dados(0), quantidade(0), capacidade(0) {}

  ~ListaVetor() { delete[] dados; }

  void inserir(int id) {
    if (quantidade == capacidade) {
      expandir();
    }
    dados[quantidade] = id;
    quantidade++;
  }

  int getQuantidade() const { return quantidade; }

  int getId(int posicao) const { return dados[posicao]; }

  long long memoriaBytes() const {
    return (long long)sizeof(ListaVetor) + (long long)capacidade * sizeof(int);
  }
};

struct NoLista {
  int id;
  NoLista *prox;
};

class ListaEncadeada {
private:
  NoLista *inicio;
  NoLista *fim;
  int quantidade;

  ListaEncadeada(const ListaEncadeada &);
  ListaEncadeada &operator=(const ListaEncadeada &);

public:
  ListaEncadeada() : inicio(0), fim(0), quantidade(0) {}

  ~ListaEncadeada() {
    NoLista *atual = inicio;
    while (atual != 0) {
      NoLista *prox = atual->prox;
      delete atual;
      atual = prox;
    }
  }

  void inserir(int id) {
    NoLista *novoNo = new NoLista;
    novoNo->id = id;
    novoNo->prox = 0;

    if (fim == 0) {
      inicio = novoNo;
      fim = novoNo;
    } else {
      fim->prox = novoNo;
      fim = novoNo;
    }

    quantidade++;
  }

  const NoLista *getInicio() const { return inicio; }

  int getQuantidade() const { return quantidade; }

  long long memoriaBytes() const {
    return (long long)sizeof(ListaEncadeada) +
           (long long)quantidade * sizeof(NoLista);
  }
};

struct EntradaVetor {
  int chave;
  bool ocupado;
  ListaVetor ids;

  EntradaVetor() : chave(0), ocupado(false), ids() {}
};

struct EntradaLista {
  int chave;
  bool ocupado;
  ListaEncadeada ids;

  EntradaLista() : chave(0), ocupado(false), ids() {}
};

class HashVetor {
private:
  EntradaVetor *tabela;
  int capacidade;
  int quantidade;

  int calcularHash(int chave) const {
    unsigned int h = (unsigned int)chave;
    h = h * 31u + 7u;
    return (int)(h % (unsigned int)capacidade);
  }

  int buscarPosicaoInsercao(int chave) const {
    int inicio = calcularHash(chave);
    int posicao = inicio;

    do {
      if (!tabela[posicao].ocupado || tabela[posicao].chave == chave) {
        return posicao;
      }
      posicao = (posicao + 1) % capacidade;
    } while (posicao != inicio);

    return -1;
  }

  void inserirDireto(int chave, int id) {
    int posicao = buscarPosicaoInsercao(chave);
    if (!tabela[posicao].ocupado) {
      tabela[posicao].chave = chave;
      tabela[posicao].ocupado = true;
      quantidade++;
    }
    tabela[posicao].ids.inserir(id);
  }

  void redimensionar() {
    int capacidadeAntiga = capacidade;
    EntradaVetor *tabelaAntiga = tabela;

    capacidade *= 2;
    tabela = new EntradaVetor[capacidade];
    quantidade = 0;

    for (int i = 0; i < capacidadeAntiga; i++) {
      if (tabelaAntiga[i].ocupado) {
        const ListaVetor &ids = tabelaAntiga[i].ids;
        for (int j = 0; j < ids.getQuantidade(); j++) {
          inserirDireto(tabelaAntiga[i].chave, ids.getId(j));
        }
      }
    }

    delete[] tabelaAntiga;
  }

  HashVetor(const HashVetor &);
  HashVetor &operator=(const HashVetor &);

public:
  HashVetor() : tabela(0), capacidade(0), quantidade(0) {}

  ~HashVetor() { delete[] tabela; }

  void inicializar(int capacidadeInicial) {
    if (capacidadeInicial < 16) {
      capacidadeInicial = 16;
    }
    capacidade = capacidadeInicial;
    quantidade = 0;
    tabela = new EntradaVetor[capacidade];
  }

  void inserir(int chave, int id) {
    if ((quantidade + 1) * 2 >= capacidade) {
      redimensionar();
    }
    inserirDireto(chave, id);
  }

  const ListaVetor *buscar(int chave) const {
    int inicio = calcularHash(chave);
    int posicao = inicio;

    do {
      if (!tabela[posicao].ocupado) {
        return 0;
      }
      if (tabela[posicao].chave == chave) {
        return &tabela[posicao].ids;
      }
      posicao = (posicao + 1) % capacidade;
    } while (posicao != inicio);

    return 0;
  }

  long long memoriaBytes() const {
    long long total =
        (long long)sizeof(HashVetor) + (long long)capacidade * sizeof(EntradaVetor);
    for (int i = 0; i < capacidade; i++) {
      if (tabela[i].ocupado) {
        total += tabela[i].ids.memoriaBytes();
      }
    }
    return total;
  }
};

class HashLista {
private:
  EntradaLista *tabela;
  int capacidade;
  int quantidade;

  int calcularHash(int chave) const {
    unsigned int h = (unsigned int)chave;
    h = h * 31u + 7u;
    return (int)(h % (unsigned int)capacidade);
  }

  int buscarPosicaoInsercao(int chave) const {
    int inicio = calcularHash(chave);
    int posicao = inicio;

    do {
      if (!tabela[posicao].ocupado || tabela[posicao].chave == chave) {
        return posicao;
      }
      posicao = (posicao + 1) % capacidade;
    } while (posicao != inicio);

    return -1;
  }

  void inserirDireto(int chave, int id) {
    int posicao = buscarPosicaoInsercao(chave);
    if (!tabela[posicao].ocupado) {
      tabela[posicao].chave = chave;
      tabela[posicao].ocupado = true;
      quantidade++;
    }
    tabela[posicao].ids.inserir(id);
  }

  void redimensionar() {
    int capacidadeAntiga = capacidade;
    EntradaLista *tabelaAntiga = tabela;

    capacidade *= 2;
    tabela = new EntradaLista[capacidade];
    quantidade = 0;

    for (int i = 0; i < capacidadeAntiga; i++) {
      if (tabelaAntiga[i].ocupado) {
        const NoLista *atual = tabelaAntiga[i].ids.getInicio();
        while (atual != 0) {
          inserirDireto(tabelaAntiga[i].chave, atual->id);
          atual = atual->prox;
        }
      }
    }

    delete[] tabelaAntiga;
  }

  HashLista(const HashLista &);
  HashLista &operator=(const HashLista &);

public:
  HashLista() : tabela(0), capacidade(0), quantidade(0) {}

  ~HashLista() { delete[] tabela; }

  void inicializar(int capacidadeInicial) {
    if (capacidadeInicial < 16) {
      capacidadeInicial = 16;
    }
    capacidade = capacidadeInicial;
    quantidade = 0;
    tabela = new EntradaLista[capacidade];
  }

  void inserir(int chave, int id) {
    if ((quantidade + 1) * 2 >= capacidade) {
      redimensionar();
    }
    inserirDireto(chave, id);
  }

  const ListaEncadeada *buscar(int chave) const {
    int inicio = calcularHash(chave);
    int posicao = inicio;

    do {
      if (!tabela[posicao].ocupado) {
        return 0;
      }
      if (tabela[posicao].chave == chave) {
        return &tabela[posicao].ids;
      }
      posicao = (posicao + 1) % capacidade;
    } while (posicao != inicio);

    return 0;
  }

  long long memoriaBytes() const {
    long long total =
        (long long)sizeof(HashLista) + (long long)capacidade * sizeof(EntradaLista);
    for (int i = 0; i < capacidade; i++) {
      if (tabela[i].ocupado) {
        total += tabela[i].ids.memoriaBytes();
      }
    }
    return total;
  }
};

class EstruturaSequencial {
private:
  Registro *registros;
  int quantidade;
  int capacidade;
  long long passos;

public:
  EstruturaSequencial(int capacidadeInicial)
      : registros(new Registro[capacidadeInicial]), quantidade(0),
        capacidade(capacidadeInicial), passos(0) {}

  ~EstruturaSequencial() { delete[] registros; }

  void inserir(const Registro &r) {
    registros[quantidade] = r;
    quantidade++;
  }

  int consultar(const Filtro *filtros, int qtdFiltros) {
    int qtdResultado = 0;

    for (int i = 0; i < quantidade; i++) {
      bool ok = true;
      for (int j = 0; j < qtdFiltros; j++) {
        passos++;
        if (registros[i].valor[filtros[j].atributo] != filtros[j].valor) {
          ok = false;
          break;
        }
      }
      if (ok) {
        qtdResultado++;
      }
    }

    return qtdResultado;
  }

  void zerarContadores() { passos = 0; }

  long long getPassos() const { return passos; }

  long long memoriaBytes() const {
    return (long long)sizeof(EstruturaSequencial) +
           (long long)capacidade * sizeof(Registro);
  }
};

class EstruturaHashVetor {
private:
  Registro *registros;
  int quantidade;
  int capacidade;
  HashVetor indices[4];
  int *resultado;
  int *auxiliar;
  long long passos;

  int intersecao(const int *a, int qtdA, const ListaVetor *b, int *saida) {
    if (b == 0) {
      return 0;
    }

    int i = 0;
    int j = 0;
    int qtdSaida = 0;

    while (i < qtdA && j < b->getQuantidade()) {
      passos++;
      int idB = b->getId(j);
      if (a[i] == idB) {
        saida[qtdSaida] = a[i];
        qtdSaida++;
        i++;
        j++;
      } else if (a[i] < idB) {
        i++;
      } else {
        j++;
      }
    }

    return qtdSaida;
  }

public:
  EstruturaHashVetor(int capacidadeInicial)
      : registros(new Registro[capacidadeInicial]), quantidade(0),
        capacidade(capacidadeInicial), resultado(new int[capacidadeInicial]),
        auxiliar(new int[capacidadeInicial]), passos(0) {
    for (int i = 0; i < 4; i++) {
      indices[i].inicializar(16);
    }
  }

  ~EstruturaHashVetor() {
    delete[] registros;
    delete[] resultado;
    delete[] auxiliar;
  }

  void inserir(const Registro &r) {
    int id = quantidade;
    registros[id] = r;
    quantidade++;

    for (int i = 0; i < 4; i++) {
      indices[i].inserir(r.valor[i], id);
    }
  }

  int consultar(const Filtro *filtros, int qtdFiltros) {
    const ListaVetor *primeiraLista =
        indices[filtros[0].atributo].buscar(filtros[0].valor);
    if (primeiraLista == 0) {
      return 0;
    }

    int qtdResultado = primeiraLista->getQuantidade();
    for (int i = 0; i < qtdResultado; i++) {
      passos++;
      resultado[i] = primeiraLista->getId(i);
    }

    for (int i = 1; i < qtdFiltros && qtdResultado > 0; i++) {
      const ListaVetor *lista = indices[filtros[i].atributo].buscar(filtros[i].valor);
      int qtdAux = intersecao(resultado, qtdResultado, lista, auxiliar);

      for (int j = 0; j < qtdAux; j++) {
        resultado[j] = auxiliar[j];
      }
      qtdResultado = qtdAux;
    }

    return qtdResultado;
  }

  void zerarContadores() { passos = 0; }

  long long getPassos() const { return passos; }

  long long memoriaBytes() const {
    long long total = (long long)sizeof(EstruturaHashVetor) +
                      (long long)capacidade * sizeof(Registro) +
                      2LL * capacidade * sizeof(int);
    for (int i = 0; i < 4; i++) {
      total += indices[i].memoriaBytes();
    }
    return total;
  }
};

class EstruturaHashLista {
private:
  Registro *registros;
  int quantidade;
  int capacidade;
  HashLista indices[4];
  int *resultado;
  int *auxiliar;
  long long passos;

  int copiarLista(const ListaEncadeada *lista, int *saida) {
    if (lista == 0) {
      return 0;
    }

    int qtd = 0;
    const NoLista *atual = lista->getInicio();
    while (atual != 0) {
      passos++;
      saida[qtd] = atual->id;
      qtd++;
      atual = atual->prox;
    }

    return qtd;
  }

  int intersecao(const int *a, int qtdA, const ListaEncadeada *b, int *saida) {
    if (b == 0) {
      return 0;
    }

    int i = 0;
    int qtdSaida = 0;
    const NoLista *atual = b->getInicio();

    while (i < qtdA && atual != 0) {
      passos++;
      if (a[i] == atual->id) {
        saida[qtdSaida] = a[i];
        qtdSaida++;
        i++;
        atual = atual->prox;
      } else if (a[i] < atual->id) {
        i++;
      } else {
        atual = atual->prox;
      }
    }

    return qtdSaida;
  }

public:
  EstruturaHashLista(int capacidadeInicial)
      : registros(new Registro[capacidadeInicial]), quantidade(0),
        capacidade(capacidadeInicial), resultado(new int[capacidadeInicial]),
        auxiliar(new int[capacidadeInicial]), passos(0) {
    for (int i = 0; i < 4; i++) {
      indices[i].inicializar(16);
    }
  }

  ~EstruturaHashLista() {
    delete[] registros;
    delete[] resultado;
    delete[] auxiliar;
  }

  void inserir(const Registro &r) {
    int id = quantidade;
    registros[id] = r;
    quantidade++;

    for (int i = 0; i < 4; i++) {
      indices[i].inserir(r.valor[i], id);
    }
  }

  int consultar(const Filtro *filtros, int qtdFiltros) {
    const ListaEncadeada *primeiraLista =
        indices[filtros[0].atributo].buscar(filtros[0].valor);
    int qtdResultado = copiarLista(primeiraLista, resultado);

    for (int i = 1; i < qtdFiltros && qtdResultado > 0; i++) {
      const ListaEncadeada *lista =
          indices[filtros[i].atributo].buscar(filtros[i].valor);
      int qtdAux = intersecao(resultado, qtdResultado, lista, auxiliar);

      for (int j = 0; j < qtdAux; j++) {
        resultado[j] = auxiliar[j];
      }
      qtdResultado = qtdAux;
    }

    return qtdResultado;
  }

  void zerarContadores() { passos = 0; }

  long long getPassos() const { return passos; }

  long long memoriaBytes() const {
    long long total = (long long)sizeof(EstruturaHashLista) +
                      (long long)capacidade * sizeof(Registro) +
                      2LL * capacidade * sizeof(int);
    for (int i = 0; i < 4; i++) {
      total += indices[i].memoriaBytes();
    }
    return total;
  }
};

template <class Estrutura>
double medirCadastro(int escala, int qtdFiltros, int repeticao,
                     long long *passos, long long *memoria, int *operacoes) {
  Estrutura estrutura(escala + escala / 2 + 10);
  Filtro filtros[4];
  int consultas = escala / 10;
  if (consultas < 50) {
    consultas = 50;
  }

  double inicio = agoraMs();
  for (int i = 0; i < escala; i++) {
    estrutura.inserir(gerarRegistro(i, escala, repeticao + 1));
  }

  for (int i = 0; i < consultas; i++) {
    gerarFiltros(filtros, qtdFiltros, i, escala, repeticao + 1);
    somaGlobal += estrutura.consultar(filtros, qtdFiltros);
  }
  double fim = agoraMs();

  *passos = estrutura.getPassos();
  *memoria = estrutura.memoriaBytes();
  *operacoes = escala + consultas;
  return fim - inicio;
}

template <class Estrutura>
double medirAtualizacoes(int escala, int qtdFiltros, int repeticao,
                         long long *passos, long long *memoria, int *operacoes) {
  Estrutura estrutura(escala + escala / 2 + 10);
  Filtro filtros[4];
  int base = escala / 2;
  int consultas = escala / 5;
  if (consultas < 80) {
    consultas = 80;
  }

  for (int i = 0; i < base; i++) {
    estrutura.inserir(gerarRegistro(i, escala, repeticao + 3));
  }
  estrutura.zerarContadores();

  double inicio = agoraMs();
  for (int i = base; i < base + escala; i++) {
    estrutura.inserir(gerarRegistro(i, escala, repeticao + 3));
    if ((i - base) % 5 == 0) {
      gerarFiltros(filtros, qtdFiltros, i, escala, repeticao + 3);
      somaGlobal += estrutura.consultar(filtros, qtdFiltros);
    }
  }

  for (int i = 0; i < consultas; i++) {
    gerarFiltros(filtros, qtdFiltros, i, escala, repeticao + 3);
    somaGlobal += estrutura.consultar(filtros, qtdFiltros);
  }
  double fim = agoraMs();

  *passos = estrutura.getPassos();
  *memoria = estrutura.memoriaBytes();
  *operacoes = escala + escala / 5 + consultas;
  return fim - inicio;
}

template <class Estrutura>
double medirConsultas(int escala, int qtdFiltros, int repeticao,
                      long long *passos, long long *memoria, int *operacoes) {
  Estrutura estrutura(escala + 10);
  Filtro filtros[4];
  int consultas = escala / 2;
  if (consultas > 1500) {
    consultas = 1500;
  }
  if (consultas < 200) {
    consultas = 200;
  }

  for (int i = 0; i < escala; i++) {
    estrutura.inserir(gerarRegistro(i, escala, repeticao + 5));
  }
  estrutura.zerarContadores();

  double inicio = agoraMs();
  for (int i = 0; i < consultas; i++) {
    gerarFiltros(filtros, qtdFiltros, i, escala, repeticao + 5);
    somaGlobal += estrutura.consultar(filtros, qtdFiltros);
  }
  double fim = agoraMs();

  *passos = estrutura.getPassos();
  *memoria = estrutura.memoriaBytes();
  *operacoes = consultas;
  return fim - inicio;
}

typedef double (*FuncaoMedida)(int, int, int, long long *, long long *, int *);

template <class Estrutura>
double cadastroWrapper(int escala, int qtdFiltros, int repeticao, long long *p,
                       long long *m, int *o) {
  return medirCadastro<Estrutura>(escala, qtdFiltros, repeticao, p, m, o);
}

template <class Estrutura>
double atualizacoesWrapper(int escala, int qtdFiltros, int repeticao, long long *p,
                           long long *m, int *o) {
  return medirAtualizacoes<Estrutura>(escala, qtdFiltros, repeticao, p, m, o);
}

template <class Estrutura>
double consultasWrapper(int escala, int qtdFiltros, int repeticao, long long *p,
                        long long *m, int *o) {
  return medirConsultas<Estrutura>(escala, qtdFiltros, repeticao, p, m, o);
}

void imprimirMedida(const std::string &perfil, const std::string &estrutura,
                    int escala, int qtdFiltros, FuncaoMedida funcao) {
  const int repeticoes = 5;
  double tempos[repeticoes];
  long long passos = 0;
  long long memoria = 0;
  int operacoes = 0;

  for (int i = 0; i < repeticoes; i++) {
    long long passosAtual = 0;
    long long memoriaAtual = 0;
    int operacoesAtual = 0;

    tempos[i] = funcao(escala, qtdFiltros, i, &passosAtual, &memoriaAtual,
                       &operacoesAtual);

    if (i == repeticoes / 2) {
      passos = passosAtual;
      memoria = memoriaAtual;
      operacoes = operacoesAtual;
    }
  }

  double tempo = mediana(tempos, repeticoes);
  double medioUs = (tempo * 1000.0) / operacoes;

  std::cout << perfil << "," << estrutura << "," << escala << "," << qtdFiltros
            << "," << operacoes << "," << tempo << "," << medioUs << ","
            << passos << "," << memoria << "\n";
}

void modoDesempenho() {
  int escalas[] = {1000, 5000, 10000, 20000};

  std::cout << "perfil,estrutura,escala,filtros,operacoes,tempo_ms,"
               "tempo_medio_us,elementos_percorridos,memoria_bytes\n";

  for (int i = 0; i < 4; i++) {
    int escala = escalas[i];

    imprimirMedida("cadastros", "varredura", escala, 2,
                   cadastroWrapper<EstruturaSequencial>);
    imprimirMedida("cadastros", "hash_vetor", escala, 2,
                   cadastroWrapper<EstruturaHashVetor>);
    imprimirMedida("cadastros", "hash_lista", escala, 2,
                   cadastroWrapper<EstruturaHashLista>);

    imprimirMedida("compras_reposicoes", "varredura", escala, 2,
                   atualizacoesWrapper<EstruturaSequencial>);
    imprimirMedida("compras_reposicoes", "hash_vetor", escala, 2,
                   atualizacoesWrapper<EstruturaHashVetor>);
    imprimirMedida("compras_reposicoes", "hash_lista", escala, 2,
                   atualizacoesWrapper<EstruturaHashLista>);

    for (int filtros = 1; filtros <= 4; filtros++) {
      imprimirMedida("consultas", "varredura", escala, filtros,
                     consultasWrapper<EstruturaSequencial>);
      imprimirMedida("consultas", "hash_vetor", escala, filtros,
                     consultasWrapper<EstruturaHashVetor>);
      imprimirMedida("consultas", "hash_lista", escala, filtros,
                     consultasWrapper<EstruturaHashLista>);
    }
  }

  if (somaGlobal == -1) {
    std::cerr << "ignorar\n";
  }
}

long long percorrerVetor(const int *dados, int n, int repeticoes) {
  long long soma = 0;
  for (int r = 0; r < repeticoes; r++) {
    for (int i = 0; i < n; i++) {
      soma += dados[i];
    }
  }
  return soma;
}

long long percorrerLista(const NoLista *inicio, int repeticoes) {
  long long soma = 0;
  for (int r = 0; r < repeticoes; r++) {
    const NoLista *atual = inicio;
    while (atual != 0) {
      soma += atual->id;
      atual = atual->prox;
    }
  }
  return soma;
}

void modoLocalidadeTempo() {
  int tamanhos[] = {100, 1000, 10000, 100000, 300000};

  std::cout << "estrutura,tamanho,repeticoes,tempo_ms,checksum\n";

  for (int t = 0; t < 5; t++) {
    int n = tamanhos[t];
    int repeticoes = 5000000 / n;
    if (repeticoes < 5) {
      repeticoes = 5;
    }

    int *vetor = new int[n];
    ListaEncadeada lista;
    for (int i = 0; i < n; i++) {
      vetor[i] = i;
      lista.inserir(i);
    }

    double iniVetor = agoraMs();
    long long somaVetor = percorrerVetor(vetor, n, repeticoes);
    double fimVetor = agoraMs();

    double iniLista = agoraMs();
    long long somaLista = percorrerLista(lista.getInicio(), repeticoes);
    double fimLista = agoraMs();

    std::cout << "vetor," << n << "," << repeticoes << ","
              << (fimVetor - iniVetor) << "," << somaVetor << "\n";
    std::cout << "lista_encadeada," << n << "," << repeticoes << ","
              << (fimLista - iniLista) << "," << somaLista << "\n";

    delete[] vetor;
  }
}

struct NoTrace {
  int id;
  NoTrace *prox;
};

void modoTraceSintetico() {
  const int n = 4096;
  int *vetor = new int[n];
  NoTrace *nos = new NoTrace[n];
  bool *usado = new bool[n];

  for (int i = 0; i < n; i++) {
    vetor[i] = i;
    nos[i].id = i;
    nos[i].prox = 0;
    usado[i] = false;
  }

  int indice = 0;
  NoTrace *inicio = 0;
  NoTrace *fim = 0;
  for (int i = 0; i < n; i++) {
    while (usado[indice]) {
      indice = (indice + 1) % n;
    }
    usado[indice] = true;

    if (inicio == 0) {
      inicio = &nos[indice];
      fim = &nos[indice];
    } else {
      fim->prox = &nos[indice];
      fim = &nos[indice];
    }

    indice = (indice + 97) % n;
  }

  std::cout << "estrutura,ordem,endereco\n";
  for (int i = 0; i < n; i++) {
    std::cout << "vetor," << i << "," << (unsigned long long)&vetor[i] << "\n";
  }

  int ordem = 0;
  NoTrace *atual = inicio;
  while (atual != 0) {
    std::cout << "lista_encadeada," << ordem << ","
              << (unsigned long long)atual << "\n";
    atual = atual->prox;
    ordem++;
  }

  delete[] vetor;
  delete[] nos;
  delete[] usado;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "uso: benchmark <desempenho|localidade-tempo|trace>\n";
    return 1;
  }

  std::string modo = argv[1];
  if (modo == "desempenho") {
    modoDesempenho();
  } else if (modo == "localidade-tempo") {
    modoLocalidadeTempo();
  } else if (modo == "trace") {
    modoTraceSintetico();
  } else {
    std::cerr << "modo desconhecido\n";
    return 1;
  }

  return 0;
}
