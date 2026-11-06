# Projetos de Estruturas de Dados (DCC221 - UFMG)

Trabalhos práticos (TPs) da disciplina **DCC221 - Estruturas de Dados** da Universidade Federal de Minas Gerais (UFMG), 2026/1.

Cada projeto é um programa em C++11 orientado a comandos, construído sobre tipos abstratos de dados (TADs) implementados à mão, sem containers da STL. Todos incluem casos de teste, análise experimental e relatório.

## Projetos

| # | Projeto | Estruturas principais | Destaques |
| :-- | :-- | :-- | :-- |
| [TP-1](TP-1/) | **Análise de Carteiras de Ações**: janela deslizante de cotações e consultas de melhores/piores ações nas carteiras dos clientes | Buffers circulares, vetores booleanos de pertinência, Mergesort, matriz de ranking linearizada (1-D) | Compara as estratégias de ranqueamento *sob demanda* e *imediata*; os benchmarks mostram onde cada uma vence |
| [TP-2](TP-2/) | **Modelagem de Redes Sociais Acadêmicas**: usuários, temas e relações de "seguir" | TAD de grafo genérico (lista de adjacência, matriz de adjacência, matriz esparsa), dicionário para mapear IDs, ordenação | Troca de representação em tempo de execução sem perder dados; comando de recomendação `P` (ponto extra) |
| [TP-3](TP-3/) | **E-commerce com Consultas Multiatributo**: usuários, produtos, compras e reposições | Índices invertidos sobre tabelas hash (sondagem linear), listas ordenadas de IDs, buffers de consulta reutilizáveis | Benchmarks contra busca sequencial e índices com lista encadeada; estudo de localidade de referência com Valgrind Lackey |

## Organização do repositório

Todos os TPs seguem a mesma estrutura:

```
TP-N/
├── README.md     # problema, projeto, complexidade, como compilar/executar/testar, resultados
├── project/      # código-fonte: Makefile, include/, src/
├── tests/        # entradas de exemplo e saídas esperadas
├── analysis/     # (TP-1, TP-2) dados e gráficos dos benchmarks
├── experiments/  # (TP-3) código, scripts, dados e gráficos dos experimentos
└── report/       # relatório (PDF + fonte LaTeX)
```

## Início rápido

Requisitos: `g++` com suporte a C++11 e `make`. Python 3 + matplotlib só são necessários para regenerar os gráficos.

```bash
cd TP-3/project
make
./bin/tp3.out < ../tests/base/1.in
```

O README de cada TP traz os comandos exatos de compilação e teste.

## Observações

- Os Makefiles são baseados no template da disciplina, de Alysson Ribeiro da Silva (UFMG, licença MIT).
- Os enunciados oficiais pertencem à equipe da disciplina e não estão incluídos. Cada README descreve o problema com suas próprias palavras.
