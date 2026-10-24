"""Seria eksperymentow do strojenia i badania parametrow.

Uruchomienie:
    python scripts/run_experiments.py                              # lista eksperymentow i instancji
    python scripts/run_experiments.py ea_pm_swap                   # jeden eksperyment na instancji extra
    python scripts/run_experiments.py ea_gen sa_iter               # kilka
    python scripts/run_experiments.py all                          # wszystkie
    python scripts/run_experiments.py ea_tour --on easy1 hard1     # na wybranych instancjach
    python scripts/run_experiments.py ea_tour --on all             # na wszystkich 7 instancjach
"""

import os
import subprocess
import sys
from pathlib import Path

import pandas as pd

import plot_parameters

ROOT = Path(__file__).resolve().parent.parent
TUNING_DIR = ROOT / "results" / "tuning"

# Krotkie nazwy instancji (do --on)
INSTANCES = {
    "easy1": "easy/berlin52_n255_uncorr-similar-weights_05.ttp",
    "easy2": "easy/berlin52_n255_bounded-strongly-corr_05.ttp",
    "medium1": "medium/kroA150_n745_uncorr-similar-weights_05.ttp",
    "medium2": "medium/kroA150_n745_bounded-strongly-corr_05.ttp",
    "hard1": "hard/a280_n1395_uncorr-similar-weights_05.ttp",
    "hard2": "hard/a280_n1395_bounded-strongly-corr_05.ttp",
    "extra": "extra/kroA100_n495_uncorr-similar-weights_05.ttp",
}
DEFAULT_INSTANCE = "extra"
RUNS = 10

# Konfiguracje po pierwszym etapie strojenia: baza dla ea_elite_tuned, ea_packing, sa_t0 i kombinacji.
# Ostateczne konfiguracje (po ea_kombinacje i sa_kombinacje) sa w main.cpp.
EA_TUNED = {"gen": 1000, "max-evals": 10000, "greedy-init": 0.5, "mut": "swap", "pm": 0.002}
SA_TUNED = {"iter": 9999, "move": "swap", "greedy-start": 1}

# nazwa: (metoda, parametr, wartosci, stale parametry). Zmieniamy jeden parametr naraz,
# reszta ma wartosci domyslne z Config.h. Wartosc moze byc slownikiem (ea_budget: pop i gen naraz).
EXPERIMENTS = {
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
    # staly budzet pop x gen = 10 000
    "ea_budget":       ("ea", "pop x gen", [{"pop": 20, "gen": 500}, {"pop": 50, "gen": 200},
                                            {"pop": 100, "gen": 100}, {"pop": 200, "gen": 50},
                                            {"pop": 500, "gen": 20}], {}),
    "greedy_packing":  ("greedy", "packing", ["simple", "smart"], {}),
    "ea_packing":      ("ea", "packing", ["simple", "smart"], EA_TUNED),
    "ea_elite_tuned":  ("ea", "elite", [0, 1, 2, 5], EA_TUNED),
    "sa_iter":         ("sa", "iter", [1000, 5000, 10000, 50000, 100000], {}),
    "sa_cooling":      ("sa", "cooling", [0.99, 0.995, 0.999, 0.9995, 0.9999], {}),
    "sa_move":         ("sa", "move", ["swap", "inversion"], {}),
    "sa_greedy_start": ("sa", "greedy-start", [0, 1], {}),
    # t0 = 0 oznacza automatyczna temperature
    "sa_t0":           ("sa", "t0", [0, 1, 10, 100, 1000], SA_TUNED),
    # Drugi etap strojenia: kolejne zmiany dodawane do konfiguracji dostrojonej
    "ea_kombinacje":   ("ea", "konfiguracja", [
        {"label": "bazowa"},
        {"label": "Pm=0.005", "pm": 0.005},
        {"label": "+Px=1", "pm": 0.005, "px": 1.0},
        {"label": "+greedy=1", "pm": 0.005, "px": 1.0, "greedy-init": 1.0},
        {"label": "+elite=5", "pm": 0.005, "px": 1.0, "greedy-init": 1.0, "elite": 5},
        {"label": "+Tour=10", "pm": 0.005, "px": 1.0, "greedy-init": 1.0, "elite": 5, "tour": 10},
        {"label": "+pop=50", "pm": 0.005, "px": 1.0, "greedy-init": 1.0, "elite": 5, "tour": 10, "pop": 50},
        {"label": "+Tour=10, inw. Pm=0.2", "mut": "inversion", "pm": 0.2, "px": 1.0, "greedy-init": 1.0, "elite": 5, "tour": 10},
        {"label": "+Tour=10, inw. Pm=0.5", "mut": "inversion", "pm": 0.5, "px": 1.0, "greedy-init": 1.0, "elite": 5, "tour": 10},
        {"label": "+Tour=10, inw. Pm=1", "mut": "inversion", "pm": 1.0, "px": 1.0, "greedy-init": 1.0, "elite": 5, "tour": 10},
    ], EA_TUNED),
    "sa_kombinacje":   ("sa", "konfiguracja", [
        {"label": "swap T0=10", "t0": 10},
        {"label": "swap T0=100", "t0": 100},
        {"label": "inw. T0=auto", "move": "inversion", "t0": 0},
        {"label": "inw. T0=10", "move": "inversion", "t0": 10},
        {"label": "inw. T0=100", "move": "inversion", "t0": 100},
        {"label": "inw. T0=1000", "move": "inversion", "t0": 1000},
        {"label": "inw. T0=100 a=0.995", "move": "inversion", "t0": 100, "cooling": 0.995},
    ], SA_TUNED),
}


def find_executable():
    """Szuka programu; Release ma pierwszenstwo, bo jest duzo szybszy."""
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


def program_environment(exe):
    """Dodaje do PATH folder kompilatora MinGW (z CMakeCache.txt), bez tego poza CLion brakuje DLL."""
    env = os.environ.copy()
    cache = exe.parent / "CMakeCache.txt"
    if cache.exists():
        for line in cache.read_text(encoding="utf-8", errors="ignore").splitlines():
            if line.startswith("CMAKE_CXX_COMPILER:"):
                compiler_dir = str(Path(line.split("=", 1)[1]).parent)
                env["PATH"] = compiler_dir + os.pathsep + env.get("PATH", "")
                break
    return env


def run_program(exe, arguments):
    command = [str(exe)] + [str(a) for a in arguments]
    completed = subprocess.run(command, capture_output=True, text=True, env=program_environment(exe))
    print(completed.stdout, end="")
    if completed.returncode != 0:
        print(completed.stderr)
        print(f"Program zakonczyl sie bledem (kod {completed.returncode}), przerywam.")
        if completed.returncode in (3221225781, -1073741515):
            print("Ten kod oznacza brak bibliotek DLL. Przebuduj projekt w CLion (Reload CMake + Build).")
        sys.exit(1)


def greedy_reference(exe, instance):
    """Najlepszy wynik zachlannego na instancji."""
    path = TUNING_DIR / "greedy_reference.csv"
    if path.exists():
        path.unlink()
    run_program(exe, ["--method", "greedy", "--instance", INSTANCES[instance], "--out", path, "--label", "greedy"])
    return pd.read_csv(path, sep=";")["best"].iloc[0]


def run_experiment(exe, name, instance, greedy_best):
    method, param, values, extra = EXPERIMENTS[name]
    out = TUNING_DIR / f"{name}__{instance}.csv"
    if out.exists():
        out.unlink()

    print(f"\n=== {name} na {instance}: {method}, zmieniamy {param} ===")
    for value in values:
        arguments = ["--method", method, "--instance", INSTANCES[instance], "--runs", RUNS, "--out", out]
        for key, extra_value in extra.items():
            arguments += ["--" + key, extra_value]

        if isinstance(value, dict):
            options = {k: v for k, v in value.items() if k != "label"}
            for key, v in options.items():
                arguments += ["--" + key, v]
            label = value.get("label", "x".join(str(v) for v in options.values()))
        else:
            arguments += ["--" + param, value]
            label = value
        arguments += ["--label", label]

        run_program(exe, arguments)

    print_summary(out)


def print_summary(csv_path):
    data = pd.read_csv(csv_path, sep=";")
    print(data[["label", "avg", "std", "best", "evaluations", "time_ms"]].to_string(index=False))


def main():
    arguments = sys.argv[1:]

    names = []
    instances = [DEFAULT_INSTANCE]
    if "--on" in arguments:
        position = arguments.index("--on")
        names = arguments[:position]
        instances = arguments[position + 1:]
    else:
        names = arguments

    if not names:
        print(__doc__)
        print("Dostepne eksperymenty:")
        for name, (method, param, values, extra) in EXPERIMENTS.items():
            print(f"  {name:18} {method:6} {param}: {values} {extra if extra else ''}")
        print("\nInstancje (do --on):")
        for short, path in INSTANCES.items():
            print(f"  {short:8} {path}")
        return

    if names == ["all"]:
        names = list(EXPERIMENTS.keys())
    if instances == ["all"]:
        instances = list(INSTANCES.keys())

    for name in names:
        if name not in EXPERIMENTS:
            print(f"Nieznany eksperyment: {name}")
            return
    for instance in instances:
        if instance not in INSTANCES:
            print(f"Nieznana instancja: {instance} (dozwolone: {', '.join(INSTANCES)} albo all)")
            return

    exe = find_executable()
    TUNING_DIR.mkdir(parents=True, exist_ok=True)

    for instance in instances:
        greedy_best = greedy_reference(exe, instance)
        for name in names:
            run_experiment(exe, name, instance, greedy_best)

    print("\n=== Wykresy i tabele ===")
    plot_parameters.main()


if __name__ == "__main__":
    main()
