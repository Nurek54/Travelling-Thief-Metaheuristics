"""Czytelne wykresy i tabele z badan parametrow.

Czyta wyniki eksperymentow z results/tuning/<eksperyment>__<instancja>.csv
(zapisuje je scripts/run_experiments.py) i tworzy:

  results/tuning/plots/<eksperyment>__<instancja>.png   wykres dla jednej instancji (wartosci bezwzgledne)
  results/tuning/plots/<eksperyment>__wszystkie.png     wszystkie instancje na jednym wykresie (znormalizowane)
  results/tuning/tabele.md                              tabele avg +/- std (podglad)
  results/tuning/tabele.tex                             te same tabele w LaTeX (do sprawozdania)

Zasady:
  - eksperymenty z 2 wartosciami (np. turniej / ruletka) sa tylko w tabelach, bez wykresu,
  - na wykresach nie ma linii algorytmu zachlannego (spłaszczalaby skale), jej wartosc jest w podtytule,
  - wykres zbiorczy normalizuje kazda instancje do [0, 1] (0 = najgorsza wartosc parametru, 1 = najlepsza),
    zeby bylo widac, czy optimum lezy w tym samym miejscu na roznych instancjach.

Uruchomienie (z glownego folderu projektu):
    python scripts/plot_parameters.py
"""

from pathlib import Path

import matplotlib
matplotlib.use("Agg")  # rysowanie do pliku, bez okna
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd

ROOT = Path(__file__).resolve().parent.parent
TUNING_DIR = ROOT / "results" / "tuning"
PLOTS_DIR = TUNING_DIR / "plots"
RESULTS_FILE = ROOT / "results" / "wyniki.csv"

# Pliki w results/tuning, ktore nie sa eksperymentami
IGNORED = {"greedy_reference", "combo", "final_check", "x", "experiments"}

# Kolejnosc i opis instancji
INSTANCES = {
    "easy1": "a280_n279_uncorr_01",
    "easy2": "a280_n279_bounded-strongly-corr_01",
    "medium1": "a280_n1395_uncorr-similar-weights_05",
    "medium2": "a280_n1395_bounded-strongly-corr_05",
    "hard1": "a280_n2790_uncorr_10",
    "hard2": "a280_n2790_bounded-strongly-corr_10",
    "extra": "a280_n837_uncorr-similar-weights_05",
}

# Czytelne tytuly i nazwy osi
TITLES = {
    "ea_gen": ("EA: liczba pokoleń", "gen"),
    "ea_pop": ("EA: rozmiar populacji", "pop_size"),
    "ea_pm_inversion": ("EA: prawdopodobieństwo mutacji (inwersja, na osobnika)", "Pm"),
    "ea_pm_swap": ("EA: prawdopodobieństwo mutacji (swap, na gen)", "Pm"),
    "ea_px": ("EA: prawdopodobieństwo krzyżowania", "Px"),
    "ea_tour": ("EA: rozmiar turnieju", "Tour"),
    "ea_elite": ("EA: elitaryzm", "liczba elitarnych osobników"),
    "ea_sel": ("EA: metoda selekcji", "selekcja"),
    "ea_cross": ("EA: operator krzyżowania", "krzyżowanie"),
    "ea_greedy_init": ("EA: udział tras zachłannych w populacji startowej", "greedyInit"),
    "ea_budget": ("EA: populacja × pokolenia przy stałym budżecie", "pop_size × gen"),
    "ea_packing": ("EA: pakowanie przedmiotów", "pakowanie"),
    "greedy_packing": ("Zachłanny: pakowanie przedmiotów", "pakowanie"),
    "sa_iter": ("SA: liczba iteracji", "iteracje"),
    "sa_cooling": ("SA: współczynnik chłodzenia", "α"),
    "sa_move": ("SA: ruch do sąsiada", "ruch"),
    "sa_greedy_start": ("SA: rozwiązanie startowe", "start (0 = losowy, 1 = zachłanny)"),
}

# Kolory i znaczniki instancji (stala kolejnosc, znacznik jako drugie kodowanie obok koloru)
STYLE = {
    "easy1": ("#2a78d6", "o"),
    "easy2": ("#2a78d6", "o"),
    "medium1": ("#eb6834", "s"),
    "medium2": ("#eb6834", "s"),
    "hard1": ("#1baf7a", "^"),
    "hard2": ("#1baf7a", "^"),
    "extra": ("#6250d6", "D"),
}
COLOR_SINGLE = "#2a78d6"
COLOR_TEXT = "#0b0b0b"
COLOR_MUTED = "#52514e"
COLOR_GRID = "#e4e3df"


# ---------------------------------------------------------------------------
# Wczytywanie
# ---------------------------------------------------------------------------

def load_experiments():
    """Zwraca slownik: eksperyment -> {instancja -> DataFrame}."""
    experiments = {}
    old_style = {}  # pliki bez "__" (stara wersja skryptu, zawsze instancja extra)

    for path in sorted(TUNING_DIR.glob("*.csv")):
        if path.stem in IGNORED:
            continue
        data = pd.read_csv(path, sep=";")
        if data.empty:
            continue
        if "__" in path.stem:
            name, instance = path.stem.split("__", 1)
            experiments.setdefault(name, {})[instance] = data
        else:
            old_style[path.stem] = data

    # Stare pliki traktujemy jako "extra", jesli nie ma nowszego pliku __extra
    for name, data in old_style.items():
        if name in TITLES and "extra" not in experiments.get(name, {}):
            experiments.setdefault(name, {})["extra"] = data

    return experiments


def load_greedy_best():
    """Najlepszy wynik zachlannego dla kazdej instancji (do podtytulu), z results/wyniki.csv."""
    if not RESULTS_FILE.exists():
        return {}
    results = pd.read_csv(RESULTS_FILE, sep=";")
    greedy = results[results["method"] == "greedy"]
    best = {}
    for _, row in greedy.iterrows():
        stem = Path(row["instance"]).stem
        for short, full in INSTANCES.items():
            if full == stem:
                best[short] = row["best"]
    return best


def sorted_instances(instances):
    order = list(INSTANCES.keys())
    return sorted(instances, key=lambda i: order.index(i) if i in order else 99)


def is_numeric(labels):
    try:
        [float(v) for v in labels]
        return True
    except ValueError:
        return False


def is_ordered(name, labels):
    """Czy wartosci parametru maja kolejnosc (wtedy laczymy punkty linia)."""
    return is_numeric(labels) or name == "ea_budget"


# ---------------------------------------------------------------------------
# Formatowanie
# ---------------------------------------------------------------------------

def thousands(value, _=None):
    return f"{value:,.0f}".replace(",", " ")


def pretty_label(value):
    """Etykieta wartosci parametru: 0.0010 -> 0.001, 100.0 -> 100, '20x500' -> '20×500'."""
    text = str(value)
    if is_numeric([text]):
        number = float(text)
        if number == int(number) and abs(number) >= 1:
            return str(int(number))
        return f"{number:g}"
    return text.replace("x", "×")


def style_axes(ax):
    ax.grid(True, color=COLOR_GRID, linewidth=0.8)
    ax.set_axisbelow(True)
    for side in ["top", "right"]:
        ax.spines[side].set_visible(False)
    for side in ["left", "bottom"]:
        ax.spines[side].set_color(COLOR_GRID)
    ax.tick_params(colors=COLOR_MUTED)


def save(fig, filename):
    PLOTS_DIR.mkdir(parents=True, exist_ok=True)
    fig.tight_layout()
    fig.savefig(PLOTS_DIR / filename)
    plt.close(fig)
    print(f"Zapisano {(PLOTS_DIR / filename).relative_to(ROOT)}")


# ---------------------------------------------------------------------------
# Wykresy
# ---------------------------------------------------------------------------

def plot_single(name, instance, data, greedy_best):
    """Jedna instancja: avg +/- std dla kazdej wartosci parametru, skala dopasowana do danych."""
    title, xlabel = TITLES.get(name, (name, "wartość"))
    labels = [pretty_label(v) for v in data["label"]]
    positions = list(range(len(labels)))

    fig, ax = plt.subplots(figsize=(7, 4), dpi=150)
    line_style = "-" if is_ordered(name, data["label"].astype(str)) else "none"
    ax.errorbar(positions, data["avg"], yerr=data["std"], color=COLOR_SINGLE, linewidth=2,
                linestyle=line_style, marker="o", markersize=7, capsize=5)

    # Najlepsza wartosc wyrozniona
    best_pos = int(data["avg"].values.argmax())
    ax.plot(positions[best_pos], data["avg"].iloc[best_pos], marker="o", markersize=12,
            markerfacecolor="none", markeredgecolor=COLOR_TEXT, markeredgewidth=1.5)

    ax.set_xticks(positions)
    ax.set_xticklabels(labels)
    ax.set_xlabel(xlabel, color=COLOR_MUTED)
    ax.set_ylabel("średnie G (± std, 10 uruchomień)", color=COLOR_MUTED)
    ax.yaxis.set_major_formatter(FuncFormatter(thousands))
    style_axes(ax)

    subtitle = f"instancja {instance}"
    if instance in greedy_best:
        subtitle += f"   |   zachłanny (best): {thousands(greedy_best[instance])}"
    ax.set_title(f"{title}\n{subtitle}", color=COLOR_TEXT, fontsize=10, loc="left")

    save(fig, f"{name}__{instance}.png")


def plot_combined(name, per_instance):
    """Wszystkie instancje razem, kazda znormalizowana do [0, 1]."""
    title, xlabel = TITLES.get(name, (name, "wartość"))

    # Wspolna lista wartosci parametru (w kolejnosci z pierwszej instancji)
    first = next(iter(per_instance.values()))
    labels = [str(v) for v in first["label"]]
    positions = list(range(len(labels)))

    fig, ax = plt.subplots(figsize=(7, 4), dpi=150)
    for instance in sorted_instances(per_instance.keys()):
        data = per_instance[instance]
        values = dict(zip(data["label"].astype(str), data["avg"]))
        avg = [values.get(label) for label in labels]
        if any(v is None for v in avg):
            continue  # inne wartosci parametru niz w pozostalych instancjach

        low, high = min(avg), max(avg)
        span = high - low if high > low else 1.0
        normalized = [(v - low) / span for v in avg]

        color, marker = STYLE.get(instance, (COLOR_SINGLE, "o"))
        ax.plot(positions, normalized, color=color, marker=marker, markersize=7, linewidth=2, label=instance)

    ax.set_xticks(positions)
    ax.set_xticklabels([pretty_label(v) for v in labels])
    ax.set_ylim(-0.08, 1.08)
    ax.set_yticks([0, 0.25, 0.5, 0.75, 1.0])
    ax.set_yticklabels(["0 (najgorzej)", "0,25", "0,5", "0,75", "1 (najlepiej)"])
    ax.set_xlabel(xlabel, color=COLOR_MUTED)
    ax.set_ylabel("średnie G, znormalizowane w instancji", color=COLOR_MUTED)
    ax.set_title(f"{title}\nwszystkie instancje (0 = najgorsza, 1 = najlepsza wartość parametru)",
                 color=COLOR_TEXT, fontsize=10, loc="left")
    style_axes(ax)
    ax.legend(frameon=False, labelcolor=COLOR_TEXT, loc="best")

    save(fig, f"{name}__wszystkie.png")


# ---------------------------------------------------------------------------
# Tabele
# ---------------------------------------------------------------------------

def latex_number(value):
    text = f"{value:,.0f}".replace(",", "\\,")
    if text.startswith("-"):
        text = "$-$" + text[1:]
    return text


def latex_label(value):
    return pretty_label(value).replace(".", "{,}").replace("×", "$\\times$")


def make_tables(experiments):
    md = ["# Wyniki badań parametrów", "",
          "Średnie G ± odchylenie standardowe z 10 uruchomień. Pogrubienie: najlepsza wartość w instancji.", ""]
    tex = ["% Tabele wygenerowane przez scripts/plot_parameters.py",
           "% Wymagane pakiety: booktabs. Wklej wybrane tabele do sprawozdania.", ""]

    for name in TITLES:
        if name not in experiments:
            continue
        per_instance = experiments[name]
        instances = sorted_instances(per_instance.keys())
        title, xlabel = TITLES[name]
        labels = [str(v) for v in next(iter(per_instance.values()))["label"]]

        # najlepsza wartosc w kazdej instancji
        best_label = {}
        for instance in instances:
            data = per_instance[instance]
            best_label[instance] = str(data.loc[data["avg"].idxmax(), "label"])

        # --- Markdown ---
        md.append(f"## {title}")
        md.append("")
        md.append(f"| {xlabel} | " + " | ".join(instances) + " |")
        md.append("|---|" + "---:|" * len(instances))
        for label in labels:
            cells = []
            for instance in instances:
                data = per_instance[instance]
                row = data[data["label"].astype(str) == label]
                if row.empty:
                    cells.append("")
                    continue
                cell = f"{thousands(row['avg'].iloc[0])} ± {thousands(row['std'].iloc[0])}"
                if label == best_label[instance]:
                    cell = f"**{cell}**"
                cells.append(cell)
            md.append(f"| {pretty_label(label)} | " + " | ".join(cells) + " |")
        md.append("")

        # --- LaTeX ---
        tex.append("\\begin{table}[H]")
        tex.append("  \\centering")
        tex.append("  \\small")
        tex.append(f"  \\caption{{{title} (avg $\\pm$ std z 10 uruchomień)}}")
        tex.append(f"  \\label{{tab:{name.replace('_', '-')}}}")
        tex.append("  \\begin{tabular}{l" + "r" * len(instances) + "}")
        tex.append("    \\toprule")
        header = xlabel.replace("_", "\\_").replace("α", "$\\alpha$").replace("×", "$\\times$")
        tex.append("    " + header + " & " + " & ".join(instances) + " \\\\")
        tex.append("    \\midrule")
        for label in labels:
            cells = []
            for instance in instances:
                data = per_instance[instance]
                row = data[data["label"].astype(str) == label]
                if row.empty:
                    cells.append("")
                    continue
                cell = f"{latex_number(row['avg'].iloc[0])} $\\pm$ {latex_number(row['std'].iloc[0])}"
                if label == best_label[instance]:
                    cell = f"\\textbf{{{cell}}}"
                cells.append(cell)
            tex.append(f"    {latex_label(label)} & " + " & ".join(cells) + " \\\\")
        tex.append("    \\bottomrule")
        tex.append("  \\end{tabular}")
        tex.append("\\end{table}")
        tex.append("")

    (TUNING_DIR / "tabele.md").write_text("\n".join(md) + "\n", encoding="utf-8")
    (TUNING_DIR / "tabele.tex").write_text("\n".join(tex) + "\n", encoding="utf-8")
    print(f"Zapisano {(TUNING_DIR / 'tabele.md').relative_to(ROOT)}")
    print(f"Zapisano {(TUNING_DIR / 'tabele.tex').relative_to(ROOT)}")


# ---------------------------------------------------------------------------

def main():
    experiments = load_experiments()
    if not experiments:
        print(f"Brak wynikow w {TUNING_DIR}. Najpierw uruchom scripts/run_experiments.py.")
        return

    greedy_best = load_greedy_best()

    for name, per_instance in experiments.items():
        values_count = len(next(iter(per_instance.values())))
        if values_count < 3:
            continue  # 2 wartosci: tylko tabela, wykres nic nie wnosi

        for instance, data in per_instance.items():
            plot_single(name, instance, data, greedy_best)
        if len(per_instance) >= 2:
            plot_combined(name, per_instance)

    make_tables(experiments)


if __name__ == "__main__":
    main()
