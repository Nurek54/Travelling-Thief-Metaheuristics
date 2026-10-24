"""Wykresy i tabele z badan parametrow.

  results/tuning/plots/<eksperyment>__<instancja>.png   wykres dla jednej instancji (wartosci bezwzgledne)
  results/tuning/plots/<eksperyment>__wszystkie.png     wszystkie instancje na jednym wykresie (znormalizowane)
  results/tuning/plots/<eksperyment>__porownanie.png    eksperymenty z 2 wariantami: roznica B wzgledem A w %
  results/tuning/plots/dwa_warianty__wszystkie.png      wszystkie eksperymenty z 2 wariantami na jednym wykresie
  results/tuning/tabele.md                              tabele avg +/- std (podglad)
  results/tuning/tabele.tex                             te same tabele w LaTeX (do sprawozdania)

Zasady:
  - eksperymenty z 2 wartosciami (np. turniej / ruletka) maja wykres slupkowy: o ile procent wariant B
    jest lepszy (+) albo gorszy (-) od wariantu A w kazdej instancji, z odchyleniem standardowym,
  - na wykresach nie ma linii algorytmu zachlannego (spłaszczalaby skale), jej wartosc jest w podtytule,
  - wykres zbiorczy normalizuje kazda instancje do [0, 1] (0 = najgorsza wartosc parametru, 1 = najlepsza),
    zeby bylo widac, czy optimum lezy w tym samym miejscu na roznych instancjach.

Uruchomienie (z glownego folderu projektu):
    python scripts/plot_parameters.py
"""

import re
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd

ROOT = Path(__file__).resolve().parent.parent
TUNING_DIR = ROOT / "results" / "tuning"
PLOTS_DIR = TUNING_DIR / "plots"
RESULTS_FILE = ROOT / "results" / "wyniki.csv"

IGNORED = {"greedy_reference", "combo", "final_check", "x", "experiments"}

INSTANCES = {
    "easy1": "berlin52_n255_uncorr-similar-weights_05",
    "easy2": "berlin52_n255_bounded-strongly-corr_05",
    "medium1": "kroA150_n745_uncorr-similar-weights_05",
    "medium2": "kroA150_n745_bounded-strongly-corr_05",
    "hard1": "a280_n1395_uncorr-similar-weights_05",
    "hard2": "a280_n1395_bounded-strongly-corr_05",
    "extra": "kroA100_n495_uncorr-similar-weights_05",
}

TITLES = {
    "ea_gen": ("EA: liczba pokoleń", "gen"),
    "ea_pop": ("EA: rozmiar populacji", "pop_size"),
    "ea_pm_inversion": ("EA: prawdopodobieństwo mutacji (inwersja, na osobnika)", "Pm"),
    "ea_pm_swap": ("EA: prawdopodobieństwo mutacji (swap, na gen)", "Pm"),
    "ea_px": ("EA: prawdopodobieństwo krzyżowania", "Px"),
    "ea_tour": ("EA: rozmiar turnieju", "Tour"),
    "ea_elite": ("EA: elitaryzm", "liczba elitarnych osobników"),
    "ea_elite_tuned": ("EA dostrojony: elitaryzm", "liczba elitarnych osobników"),
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
    "sa_t0": ("SA dostrojony: temperatura początkowa", "T0 (0 = automatyczna)"),
    "ea_kombinacje": ("EA: drugi etap strojenia (zmiany dodawane po kolei)", "konfiguracja"),
    "sa_kombinacje": ("SA: drugi etap strojenia (start zachłanny)", "konfiguracja"),
}

# Kolor i znacznik instancji (znacznik pomaga, gdy kolory sa trudne do odroznienia)
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
COLOR_BETTER = "#1baf7a"
COLOR_WORSE = "#eb6834"

VARIANT_NAMES = {
    "tournament": "turniej", "roulette": "ruletka",
    "ox": "OX", "pmx": "PMX",
    "swap": "swap", "inversion": "inwersja",
    "simple": "zwykłe", "smart": "smart",
}
START_NAMES = {"0": "start losowy", "1": "start zachłanny"}
COLOR_TEXT = "#0b0b0b"
COLOR_MUTED = "#52514e"
COLOR_GRID = "#e4e3df"


def load_experiments():
    """eksperyment -> {instancja -> DataFrame}"""
    experiments = {}
    old_style = {}  # pliki bez "__" ze starej wersji skryptu (zawsze extra)

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

    for name, data in old_style.items():
        if name in TITLES and "extra" not in experiments.get(name, {}):
            experiments.setdefault(name, {})["extra"] = data

    return experiments


def load_greedy_best():
    """Najlepszy wynik zachlannego (do podtytulu wykresu), z results/wyniki.csv."""
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
    # "x" zamieniamy tylko miedzy liczbami (20x500), zeby nie zepsuc "ox" albo "pmx"
    return re.sub(r"(?<=\d)x(?=\d)", "×", text)


def set_tick_labels(ax, labels):
    """Dlugie etykiety (np. nazwy konfiguracji) pochylone, zeby na siebie nie nachodzily."""
    if max(len(label) for label in labels) > 8:
        ax.set_xticklabels(labels, rotation=25, ha="right", fontsize=8)
    else:
        ax.set_xticklabels(labels)


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


def plot_single(name, instance, data, greedy_best):
    title, xlabel = TITLES.get(name, (name, "wartość"))
    labels = [pretty_label(v) for v in data["label"]]
    positions = list(range(len(labels)))

    fig, ax = plt.subplots(figsize=(7, 4), dpi=150)
    line_style = "-" if is_ordered(name, data["label"].astype(str)) else "none"
    ax.errorbar(positions, data["avg"], yerr=data["std"], color=COLOR_SINGLE, linewidth=2,
                linestyle=line_style, marker="o", markersize=7, capsize=5)

    best_pos = int(data["avg"].values.argmax())
    ax.plot(positions[best_pos], data["avg"].iloc[best_pos], marker="o", markersize=12,
            markerfacecolor="none", markeredgecolor=COLOR_TEXT, markeredgewidth=1.5)

    ax.set_xticks(positions)
    set_tick_labels(ax, labels)
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

    first = next(iter(per_instance.values()))
    labels = [str(v) for v in first["label"]]
    positions = list(range(len(labels)))

    fig, ax = plt.subplots(figsize=(7, 4), dpi=150)
    for instance in sorted_instances(per_instance.keys()):
        data = per_instance[instance]
        values = dict(zip(data["label"].astype(str), data["avg"]))
        avg = [values.get(label) for label in labels]
        if any(v is None for v in avg):
            continue

        low, high = min(avg), max(avg)
        span = high - low if high > low else 1.0
        normalized = [(v - low) / span for v in avg]

        color, marker = STYLE.get(instance, (COLOR_SINGLE, "o"))
        ax.plot(positions, normalized, color=color, marker=marker, markersize=7, linewidth=2, label=instance)

    ax.set_xticks(positions)
    set_tick_labels(ax, [pretty_label(v) for v in labels])
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


def variant_name(name, label):
    label = str(label)
    if name == "sa_greedy_start":
        return START_NAMES.get(label, label)
    return VARIANT_NAMES.get(label, label)


def two_variant_difference(data, instance, greedy_best):
    """Roznica B - A i jej odchylenie (z std obu wariantow) w % najlepszego wyniku zachlannego.

    Dzielimy przez wynik zachlannego, a nie przez A, bo A bywa bliskie zera albo ujemne
    (np. EA z parametrami startowymi) i wtedy procenty wychodza absurdalnie duze.
    """
    a, b = data.iloc[0], data.iloc[1]
    base = abs(greedy_best.get(instance, a["avg"])) or 1.0
    diff = (b["avg"] - a["avg"]) / base * 100
    error = (a["std"] ** 2 + b["std"] ** 2) ** 0.5 / base * 100
    return diff, error


def plot_two_variants(name, per_instance, greedy_best):
    """Jeden eksperyment z 2 wariantami: slupek na instancje, o ile % B jest lepszy/gorszy od A."""
    title, _ = TITLES.get(name, (name, "wartość"))
    instances = sorted_instances(per_instance.keys())
    first = per_instance[instances[0]]
    name_a = variant_name(name, first["label"].iloc[0])
    name_b = variant_name(name, first["label"].iloc[1])

    diffs, errors = [], []
    for instance in instances:
        diff, error = two_variant_difference(per_instance[instance], instance, greedy_best)
        diffs.append(diff)
        errors.append(error)

    fig, ax = plt.subplots(figsize=(max(4.5, 1.2 * len(instances) + 2), 4), dpi=150)
    colors = [COLOR_BETTER if d >= 0 else COLOR_WORSE for d in diffs]
    positions = list(range(len(instances)))
    ax.bar(positions, diffs, color=colors, width=0.6, yerr=errors, capsize=5,
           error_kw={"ecolor": COLOR_MUTED, "elinewidth": 1})
    ax.axhline(0, color=COLOR_MUTED, linewidth=1)

    for x, d, e in zip(positions, diffs, errors):
        offset = e + 0.03 * max(1e-9, max(abs(v) + err for v, err in zip(diffs, errors)))
        y = d + offset if d >= 0 else d - offset
        ax.text(x, y, f"{d:+.1f}%".replace(".", ","), ha="center", va="bottom" if d >= 0 else "top",
                color=COLOR_TEXT, fontsize=9)

    ax.set_xticks(positions)
    ax.set_xticklabels(instances)
    ax.set_ylabel("B − A [% wyniku zachłannego]", color=COLOR_MUTED)
    ax.set_title(f"{title}\n{name_b} względem {name_a} (+ lepiej, − gorzej; wąsy: std)",
                 color=COLOR_TEXT, fontsize=10, loc="left")
    style_axes(ax)
    ax.grid(False, axis="x")
    ax.margins(y=0.2)

    save(fig, f"{name}__porownanie.png")


def plot_two_variants_combined(two_variant_experiments, greedy_best):
    """Wszystkie eksperymenty z 2 wariantami na jednym wykresie: grupa slupkow na eksperyment."""
    names = [n for n in TITLES if n in two_variant_experiments]
    if len(names) < 2:
        return
    fig, ax = plt.subplots(figsize=(9, 4.5), dpi=150)
    labelled = set()
    for g, name in enumerate(names):
        present = sorted_instances(two_variant_experiments[name].keys())
        width = 0.8 / max(len(present), 4)
        for k, instance in enumerate(present):
            diff, error = two_variant_difference(two_variant_experiments[name][instance], instance, greedy_best)
            x = g + (k - (len(present) - 1) / 2) * width
            color, _ = STYLE.get(instance, (COLOR_SINGLE, "o"))
            hatch = "//" if instance.endswith("2") else None  # easy2/medium2/hard2 kreskowane
            ax.bar(x, diff, width=width, color=color, hatch=hatch, edgecolor="white",
                   label=instance if instance not in labelled else None,
                   yerr=error, capsize=2, error_kw={"ecolor": COLOR_MUTED, "elinewidth": 0.8})
            labelled.add(instance)

    ax.axhline(0, color=COLOR_MUTED, linewidth=1)
    group_labels = []
    for name in names:
        first = next(iter(two_variant_experiments[name].values()))
        a = variant_name(name, first["label"].iloc[0])
        b = variant_name(name, first["label"].iloc[1])
        prefix = TITLES[name][0].split(":")[0]
        group_labels.append(f"{prefix}\n{b}\nvs {a}")
    ax.set_xticks(range(len(names)))
    ax.set_xticklabels(group_labels, fontsize=8)
    ax.set_ylabel("B − A [% wyniku zachłannego]", color=COLOR_MUTED)
    ax.set_title("Eksperymenty z 2 wariantami: różnica średnich B − A (+ lepiej, − gorzej),\n"
                 "w % najlepszego wyniku zachłannego na danej instancji; wąsy: std",
                 color=COLOR_TEXT, fontsize=10, loc="left")
    style_axes(ax)
    ax.grid(False, axis="x")
    handles, labels = ax.get_legend_handles_labels()
    order = sorted(range(len(labels)), key=lambda i: sorted_instances(labels).index(labels[i]))
    ax.legend([handles[i] for i in order], [labels[i] for i in order],
              frameon=False, labelcolor=COLOR_TEXT, ncol=4, fontsize=8)

    save(fig, "dwa_warianty__wszystkie.png")


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

        best_label = {}
        for instance in instances:
            data = per_instance[instance]
            best_label[instance] = str(data.loc[data["avg"].idxmax(), "label"])

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


def main():
    experiments = load_experiments()
    if not experiments:
        print(f"Brak wynikow w {TUNING_DIR}. Najpierw uruchom scripts/run_experiments.py.")
        return

    greedy_best = load_greedy_best()

    two_variant_experiments = {}
    for name, per_instance in experiments.items():
        values_count = len(next(iter(per_instance.values())))
        if values_count == 2:
            plot_two_variants(name, per_instance, greedy_best)
            two_variant_experiments[name] = per_instance
            continue
        if values_count < 2:
            continue

        for instance, data in per_instance.items():
            plot_single(name, instance, data, greedy_best)
        if len(per_instance) >= 2:
            plot_combined(name, per_instance)

    plot_two_variants_combined(two_variant_experiments, greedy_best)
    make_tables(experiments)


if __name__ == "__main__":
    main()
