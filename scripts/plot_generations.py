"""Wykresy przebiegu algorytmow.

EA: najlepszy, sredni i najgorszy osobnik w kolejnych pokoleniach.
    Czyta results/history/<instancja>_runXX.csv -> results/plots/ea_<instancja>.png
SA: najlepsze i biezace rozwiazanie w kolejnych iteracjach.
    Czyta results/history_sa/<instancja>_runXX.csv -> results/plots/sa_<instancja>.png

Pliki CSV zapisuje program C++. Wykres to srednia ze wszystkich uruchomien.

Uruchomienie (z glownego folderu projektu):
    python scripts/plot_generations.py
"""

from pathlib import Path

import matplotlib
matplotlib.use("Agg")  # rysowanie do pliku, bez okna
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd

ROOT = Path(__file__).resolve().parent.parent
HISTORY_DIR = ROOT / "results" / "history"
HISTORY_SA_DIR = ROOT / "results" / "history_sa"
PLOTS_DIR = ROOT / "results" / "plots"
RESULTS_FILE = ROOT / "results" / "wyniki.csv"

# Kolory serii (stala kolejnosc) i kolory tekstu/siatki
COLOR_BEST = "#2a78d6"     # niebieski
COLOR_AVG = "#eb6834"      # pomaranczowy
COLOR_WORST = "#1baf7a"    # morski
COLOR_GREEDY = "#52514e"   # szary (linia odniesienia)
COLOR_TEXT = "#0b0b0b"
COLOR_MUTED = "#52514e"
COLOR_GRID = "#e4e3df"


def find_instances(folder):
    """Zwraca slownik: nazwa instancji -> lista plikow CSV z jej uruchomieniami."""
    instances = {}
    for path in sorted(folder.glob("*_run*.csv")):
        name = path.stem.rsplit("_run", 1)[0]
        instances.setdefault(name, []).append(path)
    return instances


def load_greedy_best():
    """Najlepszy wynik algorytmu zachlannego dla kazdej instancji (linia odniesienia na wykresie)."""
    if not RESULTS_FILE.exists():
        return {}
    results = pd.read_csv(RESULTS_FILE, sep=";")
    greedy = results[results["method"] == "greedy"]
    best = {}
    for _, row in greedy.iterrows():
        name = Path(row["instance"]).stem
        best[name] = row["best"]
    return best


def style_axes(ax, title, xlabel):
    """Wspolny wyglad wykresow: siatka i osie w tle, pelne liczby na osi Y."""
    ax.set_title(title, color=COLOR_TEXT, fontsize=11, loc="left")
    ax.set_xlabel(xlabel, color=COLOR_MUTED)
    ax.set_ylabel("G = zysk - czas", color=COLOR_MUTED)

    ax.grid(True, color=COLOR_GRID, linewidth=0.8)
    ax.set_axisbelow(True)
    for side in ["top", "right"]:
        ax.spines[side].set_visible(False)
    for side in ["left", "bottom"]:
        ax.spines[side].set_color(COLOR_GRID)
    ax.tick_params(colors=COLOR_MUTED)

    # Pelne liczby z odstepem co 3 cyfry (1 320 000 zamiast 1.32 i "1e6" nad osia)
    ax.yaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:,.0f}".replace(",", " ")))
    ax.legend(frameon=False, labelcolor=COLOR_TEXT)


def save_figure(fig, filename):
    fig.tight_layout()
    PLOTS_DIR.mkdir(parents=True, exist_ok=True)
    out = PLOTS_DIR / filename
    fig.savefig(out)
    plt.close(fig)
    print(f"Zapisano {out.relative_to(ROOT)}")


def plot_instance(name, files, greedy_best):
    # Wczytujemy wszystkie uruchomienia i liczymy srednia dla kazdego pokolenia
    runs = [pd.read_csv(path, sep=";") for path in files]
    all_runs = pd.concat(runs)
    mean = all_runs.groupby("generation").mean()

    fig, ax = plt.subplots(figsize=(8, 4.5), dpi=150)

    ax.plot(mean.index, mean["best"], color=COLOR_BEST, linewidth=2, label="najlepszy")
    ax.plot(mean.index, mean["avg"], color=COLOR_AVG, linewidth=2, label="średni")
    ax.plot(mean.index, mean["worst"], color=COLOR_WORST, linewidth=2, label="najgorszy")

    if name in greedy_best:
        ax.axhline(greedy_best[name], color=COLOR_GREEDY, linewidth=1.5, linestyle="--",
                   label="zachłanny (best)")

    style_axes(ax, f"EA: {name}  (średnia z {len(files)} uruchomień)", "pokolenie")
    save_figure(fig, f"ea_{name}.png")


def plot_sa_instance(name, files, greedy_best):
    runs = [pd.read_csv(path, sep=";") for path in files]
    all_runs = pd.concat(runs)
    mean = all_runs.groupby("iteration").mean()

    fig, ax = plt.subplots(figsize=(8, 4.5), dpi=150)

    ax.plot(mean.index, mean["best"], color=COLOR_BEST, linewidth=2, label="najlepszy")
    ax.plot(mean.index, mean["current"], color=COLOR_AVG, linewidth=2, label="bieżący")

    if name in greedy_best:
        ax.axhline(greedy_best[name], color=COLOR_GREEDY, linewidth=1.5, linestyle="--",
                   label="zachłanny (best)")

    style_axes(ax, f"SA: {name}  (średnia z {len(files)} uruchomień)", "iteracja")
    save_figure(fig, f"sa_{name}.png")


def main():
    greedy_best = load_greedy_best()

    ea_instances = find_instances(HISTORY_DIR)
    sa_instances = find_instances(HISTORY_SA_DIR)
    if not ea_instances and not sa_instances:
        print("Brak plikow z historia. Najpierw uruchom program C++.")
        return

    for name, files in ea_instances.items():
        plot_instance(name, files, greedy_best)
    for name, files in sa_instances.items():
        plot_sa_instance(name, files, greedy_best)


if __name__ == "__main__":
    main()
