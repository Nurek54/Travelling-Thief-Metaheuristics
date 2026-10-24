"""Tabela porownawcza metod (jak Tabela 1 w tresci zadania).

Czyta results/wyniki.csv (zapisuje go program C++) i tworzy:
  - results/tabela.md
  - results/tabela.csv (do Excela)

Uruchomienie (z glownego folderu projektu):
    python scripts/make_table.py
"""

from pathlib import Path

import pandas as pd

ROOT = Path(__file__).resolve().parent.parent
RESULTS_FILE = ROOT / "results" / "wyniki.csv"
TABLE_MD = ROOT / "results" / "tabela.md"
TABLE_CSV = ROOT / "results" / "tabela.csv"

METHOD_NAMES = {
    "random": "Losowy [10k]",
    "greedy": "Zachłanny [n]",
    "ea_start": "EA start [10x]",
    "sa_start": "SA start [10x]",
    "ea": "EA dostrojony [10x]",
    "sa": "SA dostrojony [10x]",
}
STATS = ["best", "worst", "avg", "std"]


def main():
    if not RESULTS_FILE.exists():
        print(f"Brak pliku {RESULTS_FILE}. Najpierw uruchom program C++.")
        return

    results = pd.read_csv(RESULTS_FILE, sep=";")
    results["instance"] = results["instance"].apply(lambda text: Path(text).stem)

    instances = list(dict.fromkeys(results["instance"]))
    methods = [m for m in METHOD_NAMES if m in set(results["method"])]

    # Osobna tabela dla kazdej metody, bo jedna szeroka bylaby nieczytelna
    lines = ["# Porównanie metod", ""]
    for method in methods:
        lines.append(f"## {METHOD_NAMES[method]}")
        lines.append("")
        lines.append("| instancja | best | worst | avg | std | ocen | czas [ms] |")
        lines.append("|---|---:|---:|---:|---:|---:|---:|")
        for instance in instances:
            row = results[(results["instance"] == instance) & (results["method"] == method)]
            if row.empty:
                continue
            row = row.iloc[0]
            lines.append(
                f"| {instance} | {row['best']:.2f} | {row['worst']:.2f} | {row['avg']:.2f} | "
                f"{row['std']:.2f} | {int(row['evaluations'])} | {row['time_ms']:.1f} |"
            )
        lines.append("")

    lines.append("## Średnie wyniki (avg)")
    lines.append("")
    lines.append("| instancja | " + " | ".join(METHOD_NAMES[m] for m in methods) + " |")
    lines.append("|---|" + "---:|" * len(methods))
    for instance in instances:
        cells = []
        for method in methods:
            row = results[(results["instance"] == instance) & (results["method"] == method)]
            cells.append(f"{row.iloc[0]['avg']:.2f}" if not row.empty else "")
        lines.append(f"| {instance} | " + " | ".join(cells) + " |")

    TABLE_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))
    print(f"\nZapisano {TABLE_MD.relative_to(ROOT)}")

    # CSV w ukladzie Tabeli 1: wiersz = instancja, kolumny = metoda_statystyka
    wide = results.pivot(index="instance", columns="method", values=STATS)
    wide = wide.reindex(index=instances)
    columns = []
    for method in methods:
        for stat in STATS:
            columns.append((stat, method))
    wide = wide[columns]
    wide.columns = [f"{method}_{stat}" for stat, method in columns]
    wide.to_csv(TABLE_CSV, sep=";", decimal=",", float_format="%.2f")
    print(f"Zapisano {TABLE_CSV.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
