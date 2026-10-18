"""Seria eksperymentow do strojenia i badania parametrow.

Dla wybranego parametru odpala program C++ kilka razy (kazda wartosc = 10 uruchomien),
zbiera wyniki do results/tuning/<eksperyment>.csv i rysuje wykres
"wartosc parametru -> srednie G +/- odchylenie" do results/tuning/<eksperyment>.png.

Najpierw zbuduj program w CLion (najlepiej profil Release).

Uruchomienie (z glownego folderu projektu):
    python scripts/run_experiments.py                  # lista dostepnych eksperymentow
    python scripts/run_experiments.py ea_pm_swap       # jeden eksperyment
    python scripts/run_experiments.py ea_gen sa_iter   # kilka
    python scripts/run_experiments.py all              # wszystkie
"""

import subprocess
import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")  # rysowanie do pliku, bez okna
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd

ROOT = Path(__file__).resolve().parent.parent
TUNING_DIR = ROOT / "results" / "tuning"

# Na tej instancji stroimy (instancja "extra" z tresci)
INSTANCE = "extra/a280_n837_uncorr-similar-weights_05.ttp"
RUNS = 10

# nazwa: (metoda, parametr, lista wartosci, stale dodatkowe parametry)
# Zmieniamy JEDEN parametr naraz, reszta ma wartosci domyslne z Config.h.
EXPERIMENTS = {
    # --- algorytm ewolucyjny ---
    "ea_gen":          ("ea", "gen", [50, 100, 200, 500, 1000], {}),
    "ea_pop":          ("ea", "pop", [20, 50, 100, 200, 500], {}),
    "ea_pm_inversion": ("ea", "pm", [0.0, 0.05, 0.1, 0.2, 0.5, 1.0], {"mut": "inversion"}),
    "ea_pm_swap":      ("ea", "pm", [0.0, 0.001, 0.005, 0.01, 0.05], {"mut": "swap"}),
    "ea_px":           ("ea", "px", [0.0, 0.3, 0.5, 0.7, 0.9, 1.0], {}),
    "ea_tour":         ("ea", "tour", [1, 2, 5, 10, 20, 50, 100], {}),
    "ea_elite":        ("ea", "elite", [0, 1, 2, 5, 10], {}),
    "ea_sel":          ("ea", "sel", ["tournament", "roulette"], {}),
    "ea_cross":        ("ea", "cross", ["ox", "pmx"], {}),
    "ea_greedy_init":  ("ea", "greedy-init", [0.0, 0.1, 0.5, 1.0], {}),
    # --- symulowane wyzarzanie ---
    "sa_iter":         ("sa", "iter", [1000, 5000, 10000, 50000, 100000], {}),
    "sa_cooling":      ("sa", "cooling", [0.99, 0.995, 0.999, 0.9995, 0.9999], {}),
    "sa_move":         ("sa", "move", ["swap", "inversion"], {}),
    "sa_greedy_start": ("sa", "greedy-start", [0, 1], {}),
}

# Kolory jak w plot_generations.py
COLOR_SERIES = "#2a78d6"
COLOR_GREEDY = "#52514e"
COLOR_TEXT = "#0b0b0b"
COLOR_MUTED = "#52514e"
COLOR_GRID = "#e4e3df"


def find_executable():
    """Szuka zbudowanego programu. Release jest duzo szybszy, wiec ma pierwszenstwo."""
    names = ["travelling_thief_metaheuristics.exe", "travelling_thief_metaheuristics"]
    folders = ["cmake-build-release", "cmake-build-debug"]
    for folder in folders:
        for name in names:
            path = ROOT / folder / name
            if path.exists():
                if folder == "cmake-build-debug":
                    print("UWAGA: uzywam wersji Debug, bedzie wolno. Zbuduj profil Release w CLion.\n")
                return path
    print("Nie znaleziono programu. Zbuduj projekt w CLion (profil Release).")
    sys.exit(1)


def run_program(exe, arguments):
    """Odpala program C++ z argumentami i wypisuje to, co on wypisal."""
    command = [str(exe)] + [str(a) for a in arguments]
    completed = subprocess.run(command, capture_output=True, text=True)
    print(completed.stdout, end="")
    if completed.returncode != 0:
        print(completed.stderr)
        print("Program zakonczyl sie bledem, przerywam.")
        sys.exit(1)


def greedy_reference(exe):
    """Najlepszy wynik algorytmu zachlannego na tej instancji (linia odniesienia na wykresie)."""
    path = TUNING_DIR / "greedy_reference.csv"
    if path.exists():
        path.unlink()
    run_program(exe, ["--method", "greedy", "--instance", INSTANCE, "--out", path, "--label", "greedy"])
    return pd.read_csv(path, sep=";")["best"].iloc[0]


def run_experiment(exe, name, greedy_best):
    method, param, values, extra = EXPERIMENTS[name]
    out = TUNING_DIR / f"{name}.csv"
    if out.exists():
        out.unlink()  # zaczynamy od czystego pliku

    print(f"\n=== {name}: {method}, zmieniamy --{param} ===")
    for value in values:
        arguments = ["--method", method, "--instance", INSTANCE, "--runs", RUNS,
                     "--" + param, value, "--out", out, "--label", value]
        for key, extra_value in extra.items():
            arguments += ["--" + key, extra_value]
        run_program(exe, arguments)

    plot_experiment(name, param, out, greedy_best)


def plot_experiment(name, param, csv_path, greedy_best):
    data = pd.read_csv(csv_path, sep=";")
    labels = [str(v) for v in data["label"]]
    positions = list(range(len(labels)))

    # Linia laczaca punkty tylko dla wartosci liczbowych (maja kolejnosc)
    numeric = pd.api.types.is_numeric_dtype(data["label"])
    line_style = "-" if numeric else "none"

    fig, ax = plt.subplots(figsize=(8, 4.5), dpi=150)
    ax.errorbar(positions, data["avg"], yerr=data["std"], color=COLOR_SERIES, linewidth=2,
                linestyle=line_style, marker="o", markersize=7, capsize=5, label="avg ± std (10 uruchomień)")
    ax.axhline(greedy_best, color=COLOR_GREEDY, linewidth=1.5, linestyle="--", label="zachłanny (best)")

    ax.set_xticks(positions)
    ax.set_xticklabels(labels)
    ax.set_title(f"{name}: wpływ parametru {param}", color=COLOR_TEXT, fontsize=11, loc="left")
    ax.set_xlabel(param, color=COLOR_MUTED)
    ax.set_ylabel("G = zysk - czas", color=COLOR_MUTED)

    ax.grid(True, color=COLOR_GRID, linewidth=0.8)
    ax.set_axisbelow(True)
    for side in ["top", "right"]:
        ax.spines[side].set_visible(False)
    for side in ["left", "bottom"]:
        ax.spines[side].set_color(COLOR_GRID)
    ax.tick_params(colors=COLOR_MUTED)
    ax.yaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:,.0f}".replace(",", " ")))
    ax.legend(frameon=False, labelcolor=COLOR_TEXT)
    fig.tight_layout()

    out = TUNING_DIR / f"{name}.png"
    fig.savefig(out)
    plt.close(fig)
    print(f"Zapisano {out.relative_to(ROOT)}")

    # Krotkie podsumowanie w konsoli
    print(data[["label", "avg", "std", "best", "evaluations", "time_ms"]].to_string(index=False))


def main():
    names = sys.argv[1:]
    if not names:
        print(__doc__)
        print("Dostepne eksperymenty:")
        for name, (method, param, values, extra) in EXPERIMENTS.items():
            print(f"  {name:18} {method}  --{param} {values} {extra if extra else ''}")
        return
    if names == ["all"]:
        names = list(EXPERIMENTS.keys())

    for name in names:
        if name not in EXPERIMENTS:
            print(f"Nieznany eksperyment: {name}")
            return

    exe = find_executable()
    TUNING_DIR.mkdir(parents=True, exist_ok=True)
    greedy_best = greedy_reference(exe)

    for name in names:
        run_experiment(exe, name, greedy_best)


if __name__ == "__main__":
    main()
