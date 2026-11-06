#!/usr/bin/env python3
"""
Análise experimental: Sob Demanda vs Imediata
Gera inputs sintéticos, roda ambas as estratégias e plota os resultados.
"""

import subprocess
import random
import csv
import os
import sys
import math

# ============================================================
# GERADOR DE INPUTS
# ============================================================

METRICAS = ["RET", "AVGRET", "STAB", "CONS"]

def gerar_input(N, u, w, Np, Nq, pct_carteira, num_metricas):
    """Gera uma string de input válida para o tp1."""
    random.seed(42)
    linhas = []

    # Linha M
    metricas_ativas = METRICAS[:num_metricas]
    linhas.append(f"M {w} " + " ".join(metricas_ativas))

    # Linhas A e U
    for i in range(N):
        linhas.append(f"A {i}")
    for i in range(u):
        linhas.append(f"U {i}")

    # Compras: cada cliente compra pct_carteira% das ações aleatoriamente
    qtd_compras = max(1, int(N * pct_carteira / 100))
    for c in range(u):
        acoes_cliente = random.sample(range(N), qtd_compras)
        for a in acoes_cliente:
            linhas.append(f"B {c} {a}")

    # Aquecimento: pelo menos w cotações por ação para encher a janela
    for _ in range(w):
        for a in range(N):
            preco = round(random.uniform(5.0, 200.0), 2)
            linhas.append(f"P {a} {preco}")

    # Intercala Np comandos P e Nq comandos Q
    comandos = []
    for _ in range(Np):
        acao = random.randint(0, N - 1)
        preco = round(random.uniform(5.0, 200.0), 2)
        comandos.append(f"P {acao} {preco}")

    id_consulta = 0
    for _ in range(Nq):
        cliente = random.randint(0, u - 1)
        m = random.randint(1, num_metricas)
        mets = random.sample(metricas_ativas, m)
        n_destaque = random.randint(1, min(5, qtd_compras))
        partes = [f"Q {id_consulta} {cliente} {n_destaque} {m}"]
        for met in mets:
            peso = round(random.uniform(0.5, 3.0), 2)
            partes.append(f"{met} {peso}")
        comandos.append(" ".join(partes))
        id_consulta += 1

    # Embaralha para intercalar P e Q
    random.shuffle(comandos)
    linhas.extend(comandos)

    return "\n".join(linhas) + "\n"


# ============================================================
# COMPILAÇÃO E EXECUÇÃO
# ============================================================

ANALYSIS_DIR = os.path.dirname(os.path.abspath(__file__))
CODE_DIR = os.path.join(ANALYSIS_DIR, "..", "project")
BIN_PATH = os.path.join(CODE_DIR, "bin", "tp1.out")

def compilar(estrategia):
    """Compila o projeto com a estratégia escolhida."""
    flags = "-std=c++11 -O2 -Wall -DBENCHMARK"
    if estrategia == "imediata":
        flags += " -DESTRATEGIA_IMEDIATA"

    subprocess.run(
        ["make", "clean"],
        cwd=CODE_DIR, capture_output=True
    )
    resultado = subprocess.run(
        ["make", f"CXXFLAGS={flags}", "all"],
        cwd=CODE_DIR, capture_output=True, text=True
    )
    if resultado.returncode != 0:
        print(f"ERRO na compilação ({estrategia}):")
        print(resultado.stderr)
        sys.exit(1)
    print(f"  Compilado: {estrategia}")


def rodar(input_str, repeticoes=3):
    """Roda o executável e retorna a mediana dos tempos medidos."""
    resultados = []

    for _ in range(repeticoes):
        proc = subprocess.run(
            [BIN_PATH],
            input=input_str, capture_output=True, text=True
        )
        # Parse do stderr
        dados = {"P_ops": 0, "P_ns": 0, "Q_ops": 0, "Q_ns": 0}
        for linha in proc.stderr.strip().split("\n"):
            partes = linha.split()
            if len(partes) == 4 and partes[0] == "BENCH":
                tipo = partes[1]
                ops = int(partes[2])
                ns = int(partes[3])
                dados[f"{tipo}_ops"] = ops
                dados[f"{tipo}_ns"] = ns
        resultados.append(dados)

    # Mediana pelo tempo total (P_ns + Q_ns)
    resultados.sort(key=lambda d: d["P_ns"] + d["Q_ns"])
    return resultados[len(resultados) // 2]


# ============================================================
# CENÁRIOS
# ============================================================

CENARIOS = {
    # Grupo A — Razão Nq/Np (Total operações Np+Nq = 10000 constante)
    "A1": {"N": 500, "u": 5, "w": 10, "Np": 9900, "Nq": 100,  "pct": 50, "met": 2, "rotulo": "Nq/Np=0.01"},
    "A2": {"N": 500, "u": 5, "w": 10, "Np": 9091, "Nq": 909,  "pct": 50, "met": 2, "rotulo": "Nq/Np=0.1"},
    "A3": {"N": 500, "u": 5, "w": 10, "Np": 5000, "Nq": 5000, "pct": 50, "met": 2, "rotulo": "Nq/Np=1.0"},
    "A4": {"N": 500, "u": 5, "w": 10, "Np": 909,  "Nq": 9091, "pct": 50, "met": 2, "rotulo": "Nq/Np=10"},
    "A5": {"N": 500, "u": 5, "w": 10, "Np": 100,  "Nq": 9900, "pct": 50, "met": 2, "rotulo": "Nq/Np=100"},

    # Grupo B — Escala de N
    "B1": {"N": 50,   "u": 5, "w": 10, "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "N=50"},
    "B2": {"N": 200,  "u": 5, "w": 10, "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "N=200"},
    "B3": {"N": 500,  "u": 5, "w": 10, "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "N=500"},
    "B4": {"N": 1000, "u": 5, "w": 10, "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "N=1000"},
    "B5": {"N": 2000, "u": 5, "w": 10, "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "N=2000"},

    # Grupo C — Tamanho da janela w
    "C1": {"N": 500, "u": 5, "w": 2,   "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "w=2"},
    "C2": {"N": 500, "u": 5, "w": 10,  "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "w=10"},
    "C3": {"N": 500, "u": 5, "w": 50,  "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "w=50"},
    "C4": {"N": 500, "u": 5, "w": 200, "Np": 5000, "Nq": 500, "pct": 50, "met": 2, "rotulo": "w=200"},
}


def rodar_todos():
    """Roda todos os cenários para ambas as estratégias."""
    resultados = []

    for estrategia in ["demanda", "imediata"]:
        compilar(estrategia)

        for nome, cfg in sorted(CENARIOS.items()):
            print(f"  Rodando {nome} ({cfg['rotulo']}) com {estrategia}...", end=" ", flush=True)

            input_str = gerar_input(
                cfg["N"], cfg["u"], cfg["w"],
                cfg["Np"], cfg["Nq"], cfg["pct"], cfg["met"]
            )
            dados = rodar(input_str)

            resultado = {
                "cenario": nome,
                "estrategia": estrategia,
                "rotulo": cfg["rotulo"],
                "N": cfg["N"],
                "w": cfg["w"],
                "Np": cfg["Np"],
                "Nq": cfg["Nq"],
                "P_ops": dados["P_ops"],
                "P_ms": dados["P_ns"] / 1e6,
                "P_avg_us": dados["P_ns"] / 1e3 / max(dados["P_ops"], 1),
                "Q_ops": dados["Q_ops"],
                "Q_ms": dados["Q_ns"] / 1e6,
                "Q_avg_us": dados["Q_ns"] / 1e3 / max(dados["Q_ops"], 1),
                "total_ms": (dados["P_ns"] + dados["Q_ns"]) / 1e6,
            }
            resultados.append(resultado)
            print(f"total={resultado['total_ms']:.1f}ms")

    return resultados


def salvar_csv(resultados, caminho):
    """Salva os resultados em CSV."""
    campos = ["cenario", "estrategia", "rotulo", "N", "w", "Np", "Nq",
              "P_ops", "P_ms", "P_avg_us", "Q_ops", "Q_ms", "Q_avg_us", "total_ms"]
    with open(caminho, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=campos)
        writer.writeheader()
        writer.writerows(resultados)
    print(f"\nCSV salvo em: {caminho}")


# ============================================================
# GRÁFICOS
# ============================================================

def plotar(resultados, output_dir):
    """Gera os 4 gráficos da análise experimental."""
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("matplotlib não encontrado. Instale com: pip install matplotlib")
        return

    plt.rcParams.update({
        "figure.figsize": (10, 6),
        "font.size": 12,
        "axes.grid": True,
        "grid.alpha": 0.3,
    })

    # Separa resultados por estratégia
    demanda = {r["cenario"]: r for r in resultados if r["estrategia"] == "demanda"}
    imediata = {r["cenario"]: r for r in resultados if r["estrategia"] == "imediata"}

    # ---- Gráfico 1: Crossover (Grupo A) ----
    grupo_a = ["A1", "A2", "A3", "A4", "A5"]
    razoes = [demanda[c]["Nq"] / demanda[c]["Np"] for c in grupo_a]
    tempo_dem = [demanda[c]["total_ms"] for c in grupo_a]
    tempo_ime = [imediata[c]["total_ms"] for c in grupo_a]

    fig, ax = plt.subplots()
    ax.plot(razoes, tempo_dem, "o-", label="Sob demanda", linewidth=2, markersize=8)
    ax.plot(razoes, tempo_ime, "s-", label="Imediata", linewidth=2, markersize=8)
    ax.set_xscale("log")
    ax.set_xlabel("Razão Nq/Np")
    ax.set_ylabel("Tempo total (ms)")
    ax.set_title("Sob demanda vs Imediata (Razão consultas/atualizações)")
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(output_dir, "grafico_crossover.png"), dpi=150)
    plt.close(fig)
    print("  Gerado: grafico_crossover.png")

    # ---- Gráfico 2: Escala de N (Grupo B) ----
    grupo_b = ["B1", "B2", "B3", "B4", "B5"]
    ns = [demanda[c]["N"] for c in grupo_b]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    ax1.plot(ns, [demanda[c]["P_avg_us"] for c in grupo_b], "o-", label="P demanda", linewidth=2)
    ax1.plot(ns, [imediata[c]["P_avg_us"] for c in grupo_b], "s-", label="P imediata", linewidth=2)
    ax1.set_xlabel("N (ações)")
    ax1.set_ylabel("Tempo médio por P (µs)")
    ax1.set_title("Custo por atualização de preço")
    ax1.legend()

    ax2.plot(ns, [demanda[c]["Q_avg_us"] for c in grupo_b], "o-", label="Q demanda", linewidth=2)
    ax2.plot(ns, [imediata[c]["Q_avg_us"] for c in grupo_b], "s-", label="Q imediata", linewidth=2)
    ax2.set_xlabel("N (ações)")
    ax2.set_ylabel("Tempo médio por Q (µs)")
    ax2.set_title("Custo por consulta")
    ax2.legend()

    fig.suptitle("Escala: tempo médio por operação vs N", fontsize=14, fontweight="bold")
    fig.tight_layout()
    fig.savefig(os.path.join(output_dir, "grafico_escala.png"), dpi=150)
    plt.close(fig)
    print("  Gerado: grafico_escala.png")

    # ---- Gráfico 3: Janela w (Grupo C) ----
    grupo_c = ["C1", "C2", "C3", "C4"]
    ws = [demanda[c]["w"] for c in grupo_c]
    tempo_dem_c = [demanda[c]["total_ms"] for c in grupo_c]
    tempo_ime_c = [imediata[c]["total_ms"] for c in grupo_c]

    fig, ax = plt.subplots()
    ax.plot(ws, tempo_dem_c, "o-", label="Sob demanda", linewidth=2, markersize=8)
    ax.plot(ws, tempo_ime_c, "s-", label="Imediata", linewidth=2, markersize=8)
    ax.set_xlabel("w (tamanho da janela)")
    ax.set_ylabel("Tempo total (ms)")
    ax.set_title("Impacto do tamanho da janela w")
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(output_dir, "grafico_janela.png"), dpi=150)
    plt.close(fig)
    print("  Gerado: grafico_janela.png")

    # ---- Gráfico 4: Breakdown P vs Q (A1, A3, A5) ----
    # A1 = muitos P, A3 = equilibrado, A5 = muitos Q
    cenarios_bd = ["A1", "A3", "A5"]
    labels = []
    p_tempos = []
    q_tempos = []

    for c in cenarios_bd:
        for est, dados in [("Demanda", demanda[c]), ("Imediata", imediata[c])]:
            labels.append(f"{est}\n{dados['rotulo']}")
            p_tempos.append(dados["P_ms"])
            q_tempos.append(dados["Q_ms"])

    fig, ax = plt.subplots(figsize=(12, 6))
    x = range(len(labels))
    bar_p = ax.bar(x, p_tempos, label="Tempo em P", color="#4C78A8")
    bar_q = ax.bar(x, q_tempos, bottom=p_tempos, label="Tempo em Q", color="#E45756")
    ax.set_xticks(x)
    ax.set_xticklabels(labels, fontsize=10)
    ax.set_ylabel("Tempo (ms)")
    ax.set_title("Distribuição do tempo: P vs Q por estratégia\n(A1=muitos P, A3=equilibrado, A5=muitos Q)")
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(output_dir, "grafico_breakdown.png"), dpi=150)
    plt.close(fig)
    print("  Gerado: grafico_breakdown.png")
    
    plotar_assintotica(resultados, output_dir)

def plotar_assintotica(resultados, output_dir):
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        return

    demanda = {r["cenario"]: r for r in resultados if r["estrategia"] == "demanda"}
    imediata = {r["cenario"]: r for r in resultados if r["estrategia"] == "imediata"}
    
    grupo_b = ["B1", "B2", "B3", "B4", "B5"]
    ns = [demanda[c]["N"] for c in grupo_b]
    
    # Extrair os tempos médios (µs)
    p_ime = [imediata[c]["P_avg_us"] for c in grupo_b]
    q_dem = [demanda[c]["Q_avg_us"] for c in grupo_b]
    q_ime = [imediata[c]["Q_avg_us"] for c in grupo_b]
    p_dem = [demanda[c]["P_avg_us"] for c in grupo_b]
    
    # Constantes de normalizacao (amarrando em N_max)
    N_max = ns[-1]
    
    # Teorico O(N log N)
    def fn_nlogn(n): return n * math.log2(n)
    c_p_ime = p_ime[-1] / fn_nlogn(N_max)
    c_q_dem = q_dem[-1] / fn_nlogn(N_max)
    theo_nlogn_p = [c_p_ime * fn_nlogn(n) for n in ns]
    theo_nlogn_q = [c_q_dem * fn_nlogn(n) for n in ns]

    # Teorico O(N)
    def fn_n(n): return n
    c_q_ime = q_ime[-1] / fn_n(N_max)
    theo_n_q = [c_q_ime * fn_n(n) for n in ns]

    # Teorico O(1)
    c_p_dem = p_dem[-1]
    theo_1_p = [c_p_dem for n in ns]

    fig, axs = plt.subplots(2, 2, figsize=(14, 10))

    # Plot 1: P Imediata vs O(N log N)
    axs[0, 0].plot(ns, p_ime, "s-", label="P Imediata (Empírico)", linewidth=2, color="blue")
    axs[0, 0].plot(ns, theo_nlogn_p, "--", label="O(N log N)", linewidth=2, color="orange")
    axs[0, 0].set_title("Atualização de Preço (Imediata)")
    axs[0, 0].set_ylabel("Tempo (µs)")
    axs[0, 0].legend()

    # Plot 2: Q Demanda vs O(N log N)
    axs[0, 1].plot(ns, q_dem, "o-", label="Q Demanda (Empírico)", linewidth=2, color="green")
    axs[0, 1].plot(ns, theo_nlogn_q, "--", label="O(N log N)", linewidth=2, color="orange")
    axs[0, 1].set_title("Consulta de Carteira (Demanda)")
    axs[0, 1].set_ylabel("Tempo (µs)")
    axs[0, 1].legend()

    # Plot 3: Q Imediata vs O(N)
    axs[1, 0].plot(ns, q_ime, "s-", label="Q Imediata (Empírico)", linewidth=2, color="purple")
    axs[1, 0].plot(ns, theo_n_q, "--", label="O(N)", linewidth=2, color="orange")
    axs[1, 0].set_title("Consulta de Carteira (Imediata)")
    axs[1, 0].set_xlabel("N (Ações)")
    axs[1, 0].set_ylabel("Tempo (µs)")
    axs[1, 0].legend()

    # Plot 4: P Demanda vs O(1)
    # Definindo ylim para evidenciar a constante O(1)
    axs[1, 1].plot(ns, p_dem, "o-", label="P Demanda (Empírico)", linewidth=2, color="red")
    axs[1, 1].plot(ns, theo_1_p, "--", label="O(1)", linewidth=2, color="orange")
    axs[1, 1].set_title("Atualização de Preço (Demanda)")
    axs[1, 1].set_xlabel("N (Ações)")
    axs[1, 1].set_ylim(bottom=0, top=max(p_dem)*2)
    axs[1, 1].set_ylabel("Tempo (µs)")
    axs[1, 1].legend()

    fig.suptitle("Comparação entre empírico e teórico", fontsize=16, fontweight="bold")
    fig.tight_layout()
    fig.savefig(os.path.join(output_dir, "grafico_assintotico.png"), dpi=150)
    plt.close(fig)
    print("  Gerado: grafico_assintotico.png")


# ============================================================
# MAIN
# ============================================================

if __name__ == "__main__":
    print("=" * 60)
    print("Análise Experimental: Sob Demanda vs Imediata")
    print("=" * 60)

    output_dir = ANALYSIS_DIR

    print("\n[1/3] Rodando cenários...")
    resultados = rodar_todos()

    print("\n[2/3] Salvando CSV...")
    salvar_csv(resultados, os.path.join(output_dir, "resultados.csv"))

    print("\n[3/3] Gerando gráficos...")
    plotar(resultados, output_dir)

    print(f"\nResultados salvos em: {output_dir}/")
    print("Concluído!")
