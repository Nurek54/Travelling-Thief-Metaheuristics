# Wstępne strojenie metaheurystyk

Instancja: `extra/a280_n837_uncorr-similar-weights_05.ttp` (280 miast, 837 przedmiotów).
Każda wartość parametru: **10 uruchomień** (seedy 42..51), podane avg ± std najlepszego G z uruchomienia.
Zmieniany jest **jeden parametr naraz**, pozostałe mają wartości domyślne:

- EA: `pop=100 gen=100 Px=0.7 Pm=0.1 Tour=5 elite=1 OX inwersja, start losowy`
- SA: `iter=10000 T0=auto cooling=0.999 ruch=inwersja, start losowy`

Odniesienie: algorytm zachłanny, best = **267 553**, avg = 260 597.

Wykresy: `docs/strojenie/<eksperyment>.png`. Powtórzenie: `python scripts/run_experiments.py all`.

## Algorytm ewolucyjny

| Eksperyment | Wartości → avg G | Wniosek |
|---|---|---|
| `gen` | 50: 222 581, 100: 226 396, 200: 232 395, 500: 241 718, **1000: 249 062** | więcej pokoleń stale pomaga, 100 to za mało |
| `pop` | 20: 210 581, 50: 218 468, 100: 226 396, 200: 234 130, **500: 241 295** | większa populacja pomaga i zmniejsza std, ale kosztuje więcej ocen |
| `Pm` (inwersja, na osobnika) | 0: 217 940, 0.1: 226 396, **0.5: 230 907**, 1.0: 230 076 | bez mutacji wyraźnie gorzej; najlepiej ok. 0.5 |
| `Pm` (swap, na gen) | 0: 218 409, **0.001: 235 958**, 0.005: 235 865, 0.01: 231 739, 0.05: 218 538 | swap z małym Pm lepszy od inwersji; za duże Pm niszczy trasy |
| `Px` | 0: 212 082, 0.7: 226 396, **1.0: 231 519** | krzyżowanie jest potrzebne; im więcej, tym lepiej |
| `Tour` | **1: 206 831**, **2: 228 156**, 5: 226 396, 100: 221 582 | Tour=1 to brak presji selekcyjnej (najgorzej); za duży Tour zbyt szybko ujednolica populację |
| elitaryzm | 0: 229 249, 1: 226 396, 10: 226 425 | przy tej populacji praktycznie bez wpływu |
| selekcja | **turniej: 226 396**, ruletka: 220 616 | ruletka gorsza: przy dużych G różnice procentowe są małe, presja selekcyjna słaba |
| krzyżowanie | **OX: 226 396**, PMX: 223 779 | OX nieco lepszy (zachowuje kolejność miast, co liczy się w trasie) |
| start zachłanny | 0: 226 396, 0.1: 270 402, 0.5: 271 328, **1.0: 271 390** | **największy efekt**: EA przebija algorytm zachłanny |

## Symulowane wyżarzanie

| Eksperyment | Wartości → avg G | Wniosek |
|---|---|---|
| `iter` | 1k: 207 885, 10k: 243 119, 50k: 261 226, **100k: 266 241** | więcej iteracji stale pomaga |
| `cooling` | 0.99: 242 072, **0.995: 243 913**, 0.999: 243 119, 0.9999: 213 945 | przy 10k iteracjach 0.9999 chłodzi za wolno (SA do końca błądzi losowo) |
| ruch | **swap: 260 038**, inwersja: 243 119 | swap znacznie lepszy: mniejsza zmiana trasy = dokładniejsze przeszukiwanie |
| start | losowy: 243 119, **zachłanny: 262 041** | start zachłanny daje duży skok |

## Kombinacje (te same 10 seedów)

| Konfiguracja | ocen/run | avg G | best |
|---|---|---|---|
| zachłanny (280 startów) | 280 | 260 597 | 267 553 |
| SA: swap + start zachłanny | 10 102 | 263 443 | 265 357 |
| SA: swap + start zachłanny + 50k iter | 50 102 | **271 369** | 272 629 |
| EA: greedyInit 0.5 + swap Pm 0.005 | 9 280 | 270 361 | 272 017 |
| EA: greedyInit 0.5 + swap Pm 0.005 + 500 pokoleń | 45 971 | **271 739** | 272 930 |

## Wnioski ze strojenia

1. **Inicjalizacja zachłanna** to najważniejsza zmiana: EA i SA startujące z tras najbliższego sąsiada przebijają sam algorytm zachłanny.
2. **Mutacja swap z małym Pm** (0.001-0.005 na gen) jest lepsza od inwersji, zarówno w EA, jak i jako ruch w SA.
3. **Turniej z małym Tour (2-5)** jest lepszy od ruletki i od dużego Tour.
4. Większy budżet (pokolenia, populacja, iteracje) zawsze pomaga, więc porównując metody trzeba pilnować **tej samej liczby ocen**.
