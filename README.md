# travelling-thief-metaheuristics

Metaheurystyki (losowa, zachłanna, EA, SA/TS) dla Travelling Thief Problem (TTP1).
C++ (algorytmy) + Python (wykresy, tabele).

## Postęp realizacji

### Część 1
- [x] Zapoznanie się z problemem
- [x] Implementacja loadera
- [x] Implementacja losowego osobnika TTP
- [x] Implementacja funkcji f(x)
- [x] Implementacja algorytmu zachłannego

### Część 2
- [x] Implementacja operatorów mutacji i krzyżowania
- [x] Implementacja selekcji i zarządzania populacją w algorytmie ewolucyjnym
- [x] Implementacja narzędzi logowania → wykresy
- [x] Implementacja innej metaheurystyki (SA lub TS)
- [x] Wstępne strojenie metaheurystyk (wyniki: `docs/STROJENIE.md`)
- [ ] Badanie wpływu wartości parametrów na skuteczność/efektywność algorytmu genetycznego
- [ ] Przygotowanie sprawozdania: tabelki, wykresy, wnioski

## Uruchomienie
1. Zbuduj i uruchom program C++ (najlepiej w profilu **Release**). Zapisuje:
   - `results/wyniki.csv`: best/worst/avg/std dla każdej metody i instancji,
   - `results/history/<instancja>_runXX.csv`: przebieg EA (`generation;best;avg;worst`),
   - `results/history_sa/<instancja>_runXX.csv`: przebieg SA (`iteration;best;current;temperature`).
2. Wykresy i tabela:
   ```
   pip install -r scripts/requirements.txt
   python scripts/plot_generations.py   # -> results/plots/ea_*.png, results/plots/sa_*.png
   python scripts/make_table.py         # -> results/tabela.md, results/tabela.csv
   ```

### Pojedynczy eksperyment i strojenie
Program uruchomiony z argumentami liczy jedną metodę na jednej instancji i **dopisuje** wynik do CSV w `results/`:
```
travelling_thief_metaheuristics.exe --method ea --instance extra/a280_n837_uncorr-similar-weights_05.ttp --gen 500 --mut swap --pm 0.005 --greedy-init 0.5 --runs 10 --out tuning/moje.csv
travelling_thief_metaheuristics.exe --help
```
Seria eksperymentów (jeden parametr naraz, 10 uruchomień na wartość, CSV + wykres w `results/tuning/`):
```
python scripts/run_experiments.py            # lista eksperymentów
python scripts/run_experiments.py ea_pm_swap # jeden
python scripts/run_experiments.py all        # wszystkie (ok. 7 min w Release)
```

## Założenia
- W mieście startowym złodziej nie zbiera przedmiotów (tylko z niego wyrusza).
- `RENTING RATIO` jest wczytywany, ale pomijany w funkcji oceny (zgodnie z treścią).
- Plan przedmiotów dla trasy wyznacza heurystyka zachłanna: `zysk / (waga × droga_do_końca)`.

## Struktura
- include/ttp, src: kod C++
- data: instancje .ttp (easy/medium/hard/extra)
- configs: konfiguracje eksperymentów
- results: CSV z przebiegów
- scripts: analiza wyników w Pythonie
- tests: mała instancja do ręcznej weryfikacji funkcji oceny
