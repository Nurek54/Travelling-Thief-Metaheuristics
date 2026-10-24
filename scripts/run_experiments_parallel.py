"""Te same eksperymenty co run_experiments.py, ale liczone rownolegle na wielu rdzeniach CPU.

Kazda wartosc parametru (10 uruchomien) to osobne zadanie. Kilka zadan liczy sie naraz
(osobne procesy programu C++), kazde zapisuje wynik do wlasnego pliku tymczasowego,
a na koncu pliki sa sklejane w zwykle results/tuning/<eksperyment>__<instancja>.csv.
Wyniki G sa identyczne jak w run_experiments.py (kazde uruchomienie ma swoj seed).

UWAGA: czasy (time_ms) przy liczeniu rownoleglym sa mniej wiarygodne (procesor obciazony).
Czasy do sprawozdania bierz z pelnego porownania (sam .exe bez argumentow), puszczonego osobno.

Uruchomienie (z glownego folderu projektu):
    python scripts/run_experiments_parallel.py --wszystko                  # caly zestaw badan do sprawozdania
    python scripts/run_experiments_parallel.py all --on extra              # jak w run_experiments.py
    python scripts/run_experiments_parallel.py ea_tour sa_t0 --on easy1 hard1
    python scripts/run_experiments_parallel.py --wszystko --jobs 8         # ile procesow naraz
"""

import os
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

import plot_parameters
from run_experiments import (EXPERIMENTS, INSTANCES, DEFAULT_INSTANCE, RUNS, TUNING_DIR,
                             find_executable, program_environment)

# Pelny zestaw badan do sprawozdania: (eksperymenty, instancje)
FULL_PLAN = [
    (list(EXPERIMENTS.keys()), ["extra"]),
    (["ea_pm_swap", "ea_tour", "ea_px", "ea_budget", "ea_sel", "ea_greedy_init", "ea_elite_tuned"],
     ["easy1", "medium1", "hard1"]),
    (["sa_iter", "sa_cooling", "sa_move", "sa_greedy_start", "sa_t0"], ["easy1", "medium1", "hard1"]),
    (["ea_kombinacje", "sa_kombinacje"], ["easy1", "medium1", "hard1"]),
    (["greedy_packing"], list(INSTANCES.keys())),
]

TMP_DIR = TUNING_DIR / "tmp"


def make_tasks(name, instance):
    """Jedno zadanie = jedna wartosc parametru (10 uruchomien)."""
    method, param, values, extra = EXPERIMENTS[name]
    tasks = []
    for index, value in enumerate(values):
        tmp_file = TMP_DIR / f"{name}__{instance}__{index:02d}.csv"
        arguments = ["--method", method, "--instance", INSTANCES[instance], "--runs", RUNS, "--out", tmp_file]
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

        tasks.append({"name": name, "instance": instance, "label": label,
                      "tmp_file": tmp_file, "arguments": arguments})
    return tasks


def run_task(exe, env, task):
    if task["tmp_file"].exists():
        task["tmp_file"].unlink()
    command = [str(exe)] + [str(a) for a in task["arguments"]]
    completed = subprocess.run(command, capture_output=True, text=True, env=env)
    return task, completed.returncode, completed.stderr


def merge_results(name, instance, tasks):
    """Skleja pliki tymczasowe w results/tuning/<name>__<instance>.csv w kolejnosci wartosci."""
    out = TUNING_DIR / f"{name}__{instance}.csv"
    lines = []
    for task in tasks:
        tmp_lines = task["tmp_file"].read_text(encoding="utf-8").splitlines()
        if not lines:
            lines.append(tmp_lines[0])
        lines += [line for line in tmp_lines[1:] if line.strip()]
        task["tmp_file"].unlink()
    out.write_text("\n".join(lines) + "\n", encoding="utf-8")


def parse_arguments(arguments):
    jobs = max(1, (os.cpu_count() or 2) // 2)  # polowa watkow, czyli mniej wiecej liczba rdzeni
    if "--jobs" in arguments:
        position = arguments.index("--jobs")
        jobs = int(arguments[position + 1])
        arguments = arguments[:position] + arguments[position + 2:]

    pairs = []
    if "--wszystko" in arguments:
        for names, instances in FULL_PLAN:
            for instance in instances:
                for name in names:
                    pairs.append((name, instance))
    else:
        instances = [DEFAULT_INSTANCE]
        names = arguments
        if "--on" in arguments:
            position = arguments.index("--on")
            names = arguments[:position]
            instances = arguments[position + 1:]
        if names == ["all"]:
            names = list(EXPERIMENTS.keys())
        if instances == ["all"]:
            instances = list(INSTANCES.keys())
        for name in names:
            if name not in EXPERIMENTS:
                print(f"Nieznany eksperyment: {name}")
                sys.exit(1)
        for instance in instances:
            if instance not in INSTANCES:
                print(f"Nieznana instancja: {instance} (dozwolone: {', '.join(INSTANCES)} albo all)")
                sys.exit(1)
        for instance in instances:
            for name in names:
                pairs.append((name, instance))

    # greedy_packing na extra jest w planie dwa razy
    unique_pairs = []
    for pair in pairs:
        if pair not in unique_pairs:
            unique_pairs.append(pair)
    return unique_pairs, jobs


def main():
    arguments = sys.argv[1:]
    if not arguments:
        print(__doc__)
        return

    pairs, jobs = parse_arguments(arguments)
    exe = find_executable()
    env = program_environment(exe)
    TMP_DIR.mkdir(parents=True, exist_ok=True)

    # Najdluzsze zadania (duze instancje) na poczatek, zeby na koncu nie czekac na jedno
    tasks_by_pair = {pair: make_tasks(*pair) for pair in pairs}
    all_tasks = [task for tasks in tasks_by_pair.values() for task in tasks]
    order = {"hard1": 0, "hard2": 0, "medium1": 1, "medium2": 1, "extra": 2, "easy1": 3, "easy2": 3}
    all_tasks.sort(key=lambda task: order.get(task["instance"], 9))

    print(f"Eksperymentow: {len(pairs)}, zadan: {len(all_tasks)}, procesow naraz: {jobs}\n")
    start = time.time()
    done = 0
    failed = []

    with ThreadPoolExecutor(max_workers=jobs) as pool:
        futures = [pool.submit(run_task, exe, env, task) for task in all_tasks]
        for future in as_completed(futures):
            task, code, error = future.result()
            done += 1
            status = "ok" if code == 0 else f"BLAD (kod {code})"
            print(f"[{done}/{len(all_tasks)}] {task['name']} na {task['instance']}, {task['label']}: {status}")
            if code != 0:
                failed.append((task, code, error))

    if failed:
        print("\nNiektore zadania zakonczyly sie bledem:")
        for task, code, error in failed:
            print(f"  {task['name']} {task['instance']} {task['label']} (kod {code}): {error.strip()}")
        if any(code in (3221225781, -1073741515) for _, code, _ in failed):
            print("Ten kod oznacza brak bibliotek DLL. Przebuduj projekt w CLion (Build -> Rebuild Project).")
        sys.exit(1)

    for (name, instance), tasks in tasks_by_pair.items():
        merge_results(name, instance, tasks)

    print(f"\nGotowe w {time.time() - start:.0f} s. Wyniki: results/tuning/")
    print("\n=== Wykresy i tabele ===")
    plot_parameters.main()


if __name__ == "__main__":
    main()
