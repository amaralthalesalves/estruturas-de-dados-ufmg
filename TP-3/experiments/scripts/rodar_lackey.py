#!/usr/bin/env python3
import csv
import os
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
EXP = ROOT / "experiments"
ENTRADAS = EXP / "inputs"
RESULTADOS = EXP / "results"
GRAFICOS = EXP / "plots"
TP_BIN = ROOT / "project" / "bin" / "tp3.out"

os.environ.setdefault("MPLCONFIGDIR", str(RESULTADOS / ".mplconfig"))


def garantir_pastas():
    ENTRADAS.mkdir(parents=True, exist_ok=True)
    RESULTADOS.mkdir(parents=True, exist_ok=True)
    GRAFICOS.mkdir(parents=True, exist_ok=True)
    (RESULTADOS / ".mplconfig").mkdir(parents=True, exist_ok=True)


def gerar_entrada_tp(caminho, n_usuarios=80, n_produtos=50, n_operacoes=160):
    cidades = ["BH", "Rio", "SaoPaulo", "Recife", "Curitiba", "Vitoria"]
    estados = ["MG", "RJ", "SP", "PE", "PR", "ES"]
    nacionalidades = ["BR", "AR", "UY", "CL"]
    categorias = ["Info", "Livros", "Moveis", "Casa", "Jogos"]
    marcas = ["Acer", "Dell", "Flex", "Pearson", "Sony", "Logi"]
    condicoes = ["Novo", "Usado"]

    with caminho.open("w") as arq:
        for i in range(n_usuarios):
            arq.write(
                "U Usuario{} {} {} {} {}\n".format(
                    i,
                    18 + (i * 7) % 50,
                    cidades[i % len(cidades)],
                    estados[i % len(estados)],
                    nacionalidades[i % len(nacionalidades)],
                )
            )

        for i in range(n_produtos):
            preco = 10 + (i * 13) % 900
            centavos = (i * 17) % 100
            arq.write(
                "P Produto{} {}.{:02d} {} {} {} {}\n".format(
                    i,
                    preco,
                    centavos,
                    20 + (i * 5) % 80,
                    categorias[i % len(categorias)],
                    marcas[i % len(marcas)],
                    condicoes[i % len(condicoes)],
                )
            )

        for i in range(n_operacoes):
            timestamp = 1000 + i
            produto_a = (i * 7) % n_produtos
            produto_b = (i * 11 + 3) % n_produtos
            usuario = (i * 5) % n_usuarios

            if i % 10 in (0, 1, 2):
                arq.write(f"R {timestamp} {produto_a} 3 {produto_b} 2\n")
            elif i % 10 in (3, 4, 5, 6):
                arq.write(f"C {timestamp} {usuario} {produto_a} 1 {produto_b} 1\n")
            elif i % 10 == 7:
                arq.write(f"LU cidade {cidades[i % len(cidades)]} estado {estados[i % len(estados)]}\n")
            elif i % 10 == 8:
                arq.write(
                    "LP categoria {} marca {}\n".format(
                        categorias[i % len(categorias)], marcas[i % len(marcas)]
                    )
                )
            else:
                arq.write(f"LC id_produto {produto_a}\n")


def executar_make():
    subprocess.run(["make", "-C", str(ROOT / "project"), "all"], check=True)


def executar_lackey(nome, argumentos, entrada):
    log = RESULTADOS / nome
    saida = RESULTADOS / nome.replace(".txt", "_saida.txt")
    comando = [
        "valgrind",
        "--tool=lackey",
        *argumentos,
        f"--log-file={log}",
        str(TP_BIN),
    ]

    with entrada.open("r") as arq_in, saida.open("w") as arq_out:
        subprocess.run(comando, stdin=arq_in, stdout=arq_out, stderr=subprocess.PIPE, check=True)

    return log


def parse_trace_lackey(log_trace, csv_saida, limite=25000):
    linhas = []
    ordem = 0

    with log_trace.open() as arq:
        for linha in arq:
            linha = linha.strip()
            if not linha:
                continue

            partes = linha.split()
            if len(partes) < 2:
                continue

            tipo = partes[0]
            if tipo not in ("I", "L", "S", "M"):
                continue

            endereco_tamanho = partes[1]
            if "," not in endereco_tamanho:
                continue

            endereco_txt, tamanho_txt = endereco_tamanho.split(",", 1)
            try:
                endereco = int(endereco_txt, 16)
                tamanho = int(tamanho_txt)
            except ValueError:
                continue

            linhas.append(
                {
                    "ordem": ordem,
                    "tipo": tipo,
                    "endereco": endereco,
                    "tamanho": tamanho,
                }
            )
            ordem += 1

            if ordem >= limite:
                break

    with csv_saida.open("w", newline="") as arq:
        escritor = csv.DictWriter(arq, fieldnames=["ordem", "tipo", "endereco", "tamanho"])
        escritor.writeheader()
        escritor.writerows(linhas)

    return linhas


def numero_lackey(texto):
    return int(texto.replace(",", ""))


def parse_basic_lackey(log_basic, csv_saida):
    campos = [
        "jcc_total",
        "jcc_taken",
        "sbs_entered",
        "sbs_completed",
        "guest_instrs",
        "irstmts",
    ]
    valores = {}
    lendo_jccs = False

    with log_basic.open() as arq:
        for linha in arq:
            if "Jccs:" in linha:
                lendo_jccs = True
                continue
            if lendo_jccs and "total:" in linha:
                valores["jcc_total"] = numero_lackey(linha.split("total:", 1)[1].strip().split()[0])
            if lendo_jccs and "taken:" in linha:
                valores["jcc_taken"] = numero_lackey(linha.split("taken:", 1)[1].strip().split()[0])
                lendo_jccs = False
            if "SBs entered:" in linha:
                valores["sbs_entered"] = numero_lackey(
                    linha.split("SBs entered:", 1)[1].strip().split()[0]
                )
            if "SBs completed:" in linha:
                valores["sbs_completed"] = numero_lackey(
                    linha.split("SBs completed:", 1)[1].strip().split()[0]
                )
            if "guest instrs:" in linha:
                valores["guest_instrs"] = numero_lackey(
                    linha.split("guest instrs:", 1)[1].strip().split()[0]
                )
            if "IRStmts:" in linha:
                valores["irstmts"] = numero_lackey(
                    linha.split("IRStmts:", 1)[1].strip().split()[0]
                )

    with csv_saida.open("w", newline="") as arq:
        escritor = csv.DictWriter(arq, fieldnames=campos)
        escritor.writeheader()
        escritor.writerow({nome: valores.get(nome, 0) for nome in campos})


def parse_detailed_lackey(log_detailed, csv_saida):
    linhas = []
    lendo_tabela = False

    with log_detailed.open() as arq:
        for linha in arq:
            limpa = linha.strip()
            if limpa.startswith("=="):
                fim_prefixo = limpa.find("==", 2)
                if fim_prefixo != -1:
                    limpa = limpa[fim_prefixo + 2 :].strip()
            if limpa.startswith("Type"):
                lendo_tabela = True
                continue
            if not lendo_tabela:
                continue
            if not limpa or limpa.startswith("-"):
                continue

            partes = limpa.split()
            if len(partes) != 4:
                continue
            linhas.append(
                {
                    "tipo": partes[0],
                    "loads": numero_lackey(partes[1]),
                    "stores": numero_lackey(partes[2]),
                    "aluops": numero_lackey(partes[3]),
                }
            )

    with csv_saida.open("w", newline="") as arq:
        escritor = csv.DictWriter(arq, fieldnames=["tipo", "loads", "stores", "aluops"])
        escritor.writeheader()
        escritor.writerows(linhas)


def gerar_grafico_trace_lackey(linhas):
    import matplotlib.pyplot as plt

    linhas = [l for l in linhas if l["tipo"] in ("L", "S", "M")]
    if not linhas:
        return

    paginas = sorted(set(l["endereco"] // 4096 for l in linhas))
    pagina_para_indice = {pagina: i for i, pagina in enumerate(paginas)}
    por_tipo = {"L": [], "S": [], "M": []}
    for linha in linhas:
        por_tipo[linha["tipo"]].append(linha)

    nomes = {
        "L": "Leitura",
        "S": "Escrita",
        "M": "Modificação",
    }
    cores = {
        "L": "#1f77b4",
        "S": "#d62728",
        "M": "#2ca02c",
    }

    fig, ax = plt.subplots(figsize=(8.2, 4.6))
    for tipo, dados in por_tipo.items():
        if not dados:
            continue
        xs = [l["ordem"] for l in dados]
        ys = [pagina_para_indice[l["endereco"] // 4096] for l in dados]
        ax.scatter(xs, ys, s=4, alpha=0.55, label=nomes[tipo], color=cores[tipo])

    ax.set_title("Localidade com Lackey: páginas de memória acessadas ao longo da execução")
    ax.set_ylabel("Página de memória relativa")
    ax.set_xlabel("Ordem dos acessos à memória")
    ax.legend(markerscale=3)
    ax.grid(alpha=0.25)
    fig.tight_layout()
    fig.savefig(GRAFICOS / "06_localidade_trace.png", dpi=180)
    plt.close(fig)


def main():
    garantir_pastas()
    entrada = ENTRADAS / "lackey_tp3.in"
    gerar_entrada_tp(entrada)
    executar_make()

    try:
        basic = executar_lackey(
            "lackey_basic.txt",
            ["--basic-counts=yes"],
            entrada,
        )
        detailed = executar_lackey(
            "lackey_detailed.txt",
            ["--basic-counts=no", "--detailed-counts=yes"],
            entrada,
        )
        trace = executar_lackey(
            "lackey_trace_mem.txt",
            ["--basic-counts=no", "--trace-mem=yes"],
            entrada,
        )
    except subprocess.CalledProcessError as exc:
        print("Valgrind/Lackey falhou neste ambiente.")
        print("Comando:", " ".join(exc.cmd))
        if exc.stderr:
            print(exc.stderr.decode(errors="ignore"))
        print("Os demais experimentos continuam disponiveis em rodar_experimentos.py.")
        raise SystemExit(1)

    parse_basic_lackey(basic, RESULTADOS / "lackey_basic_counts.csv")
    parse_detailed_lackey(detailed, RESULTADOS / "lackey_detailed_counts.csv")
    linhas = parse_trace_lackey(trace, RESULTADOS / "lackey_trace_amostra.csv")
    trace.unlink()
    gerar_grafico_trace_lackey(linhas)

    print("Logs Lackey gerados em:", RESULTADOS)
    print("Grafico 06 atualizado com trace real em:", GRAFICOS / "06_localidade_trace.png")


if __name__ == "__main__":
    main()
