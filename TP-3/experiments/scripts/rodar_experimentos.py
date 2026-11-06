#!/usr/bin/env python3
import csv
import os
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
EXP = ROOT / "experiments"
SRC = EXP / "src" / "benchmark.cpp"
RESULTADOS = EXP / "results"
GRAFICOS = EXP / "plots"
BIN = RESULTADOS / "benchmark"

os.environ.setdefault("MPLCONFIGDIR", str(RESULTADOS / ".mplconfig"))

import matplotlib.pyplot as plt


NOMES = {
    "varredura": "Busca sequencial em vetor",
    "hash_vetor": "Hash + vetor de ids",
    "hash_lista": "Hash + lista encadeada",
    "vetor": "Vetor",
    "lista_encadeada": "Lista encadeada",
    "cadastros": "Cadastros",
    "compras_reposicoes": "Compras/reposições",
    "consultas": "Consultas",
}


def garantir_pastas():
    RESULTADOS.mkdir(parents=True, exist_ok=True)
    GRAFICOS.mkdir(parents=True, exist_ok=True)
    (RESULTADOS / ".mplconfig").mkdir(parents=True, exist_ok=True)


def executar(comando, saida=None):
    with subprocess.Popen(
        comando,
        cwd=str(ROOT),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    ) as proc:
        out, err = proc.communicate()
        if proc.returncode != 0:
            raise RuntimeError(
                "comando falhou:\n"
                + " ".join(comando)
                + "\n\nstdout:\n"
                + out
                + "\n\nstderr:\n"
                + err
            )

    if saida is not None:
        saida.write_text(out)
    return out


def compilar_benchmark():
    comando = [
        "g++",
        "-std=c++11",
        "-O2",
        "-Wall",
        "-Wextra",
        str(SRC),
        "-o",
        str(BIN),
    ]
    executar(comando)


def gerar_csvs():
    executar([str(BIN), "desempenho"], RESULTADOS / "desempenho.csv")
    executar([str(BIN), "localidade-tempo"], RESULTADOS / "localidade_tempo.csv")
    executar([str(BIN), "trace"], RESULTADOS / "localidade_trace.csv")


def ler_csv(nome):
    caminho = RESULTADOS / nome
    with caminho.open(newline="") as arq:
        return list(csv.DictReader(arq))


def filtrar(linhas, **criterios):
    resultado = []
    for linha in linhas:
        ok = True
        for chave, valor in criterios.items():
            if str(linha[chave]) != str(valor):
                ok = False
                break
        if ok:
            resultado.append(linha)
    return resultado


def salvar(fig, nome):
    fig.tight_layout()
    fig.savefig(GRAFICOS / nome, dpi=180)
    plt.close(fig)


def grafico_tempo_perfis(linhas):
    escala = max(int(l["escala"]) for l in linhas)
    perfis = ["cadastros", "compras_reposicoes", "consultas"]
    estruturas = ["varredura", "hash_vetor", "hash_lista"]

    fig, ax = plt.subplots(figsize=(8.2, 4.6))
    largura = 0.24
    xs = list(range(len(perfis)))

    for desloc, estrutura in enumerate(estruturas):
        valores = []
        for perfil in perfis:
            candidatos = filtrar(
                linhas, perfil=perfil, estrutura=estrutura, escala=escala, filtros=2
            )
            valores.append(float(candidatos[0]["tempo_ms"]))

        posicoes = [x + (desloc - 1) * largura for x in xs]
        ax.bar(
            posicoes,
            valores,
            width=largura,
            label=NOMES[estrutura],
        )

    ax.set_title(f"Tempo total por perfil de carga ({escala:,} registros gerados)".replace(",", "."))
    ax.set_ylabel("Tempo total (ms)")
    ax.set_xlabel("Perfil de carga")
    ax.set_yscale("log")
    ax.set_xticks(xs)
    ax.set_xticklabels([NOMES[p] for p in perfis])
    ax.legend()
    ax.grid(axis="y", alpha=0.25)
    salvar(fig, "01_tempo_por_perfil.png")


def grafico_consultas_por_escala(linhas):
    estruturas = ["varredura", "hash_vetor", "hash_lista"]
    fig, ax = plt.subplots(figsize=(8.2, 4.6))

    for estrutura in estruturas:
        dados = filtrar(linhas, perfil="consultas", estrutura=estrutura, filtros=2)
        dados.sort(key=lambda l: int(l["escala"]))
        xs = [int(l["escala"]) for l in dados]
        ys = [float(l["tempo_medio_us"]) for l in dados]
        ax.plot(xs, ys, marker="o", label=NOMES[estrutura])

    ax.set_title("Tempo médio de consulta por número de registros")
    ax.set_ylabel("Tempo médio por consulta (us)")
    ax.set_xlabel("Número de registros gerados")
    ax.legend()
    ax.grid(alpha=0.25)
    salvar(fig, "02_consultas_por_escala.png")


def grafico_consultas_por_filtros(linhas):
    escala = max(int(l["escala"]) for l in linhas)
    estruturas = ["varredura", "hash_vetor", "hash_lista"]
    fig, ax = plt.subplots(figsize=(8.2, 4.6))

    for estrutura in estruturas:
        dados = filtrar(linhas, perfil="consultas", estrutura=estrutura, escala=escala)
        dados.sort(key=lambda l: int(l["filtros"]))
        xs = [int(l["filtros"]) for l in dados]
        ys = [float(l["tempo_medio_us"]) for l in dados]
        ax.plot(xs, ys, marker="o", label=NOMES[estrutura])

    ax.set_title(
        f"Tempo médio de consulta por quantidade de filtros ({escala:,} registros gerados)".replace(",", ".")
    )
    ax.set_ylabel("Tempo médio por consulta (us)")
    ax.set_xlabel("Quantidade de filtros")
    ax.set_xticks([1, 2, 3, 4])
    ax.legend()
    ax.grid(alpha=0.25)
    salvar(fig, "03_consultas_por_filtros.png")


def grafico_memoria(linhas):
    estruturas = ["varredura", "hash_vetor", "hash_lista"]
    fig, ax = plt.subplots(figsize=(8.2, 4.6))

    for estrutura in estruturas:
        dados = filtrar(linhas, perfil="consultas", estrutura=estrutura, filtros=2)
        dados.sort(key=lambda l: int(l["escala"]))
        xs = [int(l["escala"]) for l in dados]
        ys = [float(l["memoria_bytes"]) / (1024 * 1024) for l in dados]
        ax.plot(xs, ys, marker="o", label=NOMES[estrutura])

    ax.set_title("Memória aproximada por número de registros")
    ax.set_ylabel("Memória aproximada (MiB)")
    ax.set_xlabel("Número de registros gerados")
    ax.legend()
    ax.grid(alpha=0.25)
    salvar(fig, "04_memoria_aproximada.png")


def grafico_localidade_tempo(linhas):
    fig, ax = plt.subplots(figsize=(8.2, 4.6))

    for estrutura in ["vetor", "lista_encadeada"]:
        dados = [l for l in linhas if l["estrutura"] == estrutura]
        dados.sort(key=lambda l: int(l["tamanho"]))
        xs = [int(l["tamanho"]) for l in dados]
        ys = [float(l["tempo_ms"]) for l in dados]
        ax.plot(xs, ys, marker="o", label=NOMES[estrutura])

    ax.set_title("Localidade de referência: tempo para percorrer identificadores")
    ax.set_ylabel("Tempo total (ms)")
    ax.set_xlabel("Quantidade de identificadores percorridos")
    ax.set_xscale("log")
    ax.legend()
    ax.grid(alpha=0.25)
    salvar(fig, "05_localidade_tempo.png")


def grafico_localidade_trace(linhas):
    fig, ax = plt.subplots(figsize=(8.2, 4.6))

    for estrutura in ["vetor", "lista_encadeada"]:
        dados = [l for l in linhas if l["estrutura"] == estrutura]
        dados.sort(key=lambda l: int(l["ordem"]))

        enderecos = [int(l["endereco"]) for l in dados]
        menor = min(enderecos)
        xs = [int(l["ordem"]) for l in dados]
        ys = [(end - menor) / 1024.0 for end in enderecos]

        ax.scatter(
            xs,
            ys,
            s=4,
            alpha=0.65,
            label=NOMES[estrutura],
        )

    ax.set_title("Localidade de referência: ordem de acesso x posição de memória")
    ax.set_ylabel("Endereço relativo (KiB)")
    ax.set_xlabel("Ordem dos acessos à memória")
    ax.legend(markerscale=3)
    ax.grid(alpha=0.25)
    salvar(fig, "06_localidade_trace.png")


def gerar_graficos():
    desempenho = ler_csv("desempenho.csv")
    localidade_tempo = ler_csv("localidade_tempo.csv")
    localidade_trace = ler_csv("localidade_trace.csv")

    grafico_tempo_perfis(desempenho)
    grafico_consultas_por_escala(desempenho)
    grafico_consultas_por_filtros(desempenho)
    grafico_memoria(desempenho)
    grafico_localidade_tempo(localidade_tempo)
    grafico_localidade_trace(localidade_trace)


def main():
    garantir_pastas()
    compilar_benchmark()
    gerar_csvs()
    gerar_graficos()

    print("CSVs gerados em:", RESULTADOS)
    print("Graficos gerados em:", GRAFICOS)


if __name__ == "__main__":
    main()
