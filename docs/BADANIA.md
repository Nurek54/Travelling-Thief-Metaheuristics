# Badania parametrów i porównanie metod

Wszystkie liczby: **avg ± std najlepszego G z 10 uruchomień** (seedy 42..51).
Wykresy: `docs/badania/<eksperyment>__<instancja>.png`, wyniki porównania: `docs/wyniki/`.
Powtórzenie: `python scripts/run_experiments.py <eksperyment> --on easy1 medium1 hard1 extra`.

Instancje: `easy1` = a280_n279_uncorr_01, `medium1` = a280_n1395_uncorr-similar-weights_05,
`hard1` = a280_n2790_uncorr_10, `extra` = a280_n837_uncorr-similar-weights_05.

Bazowa konfiguracja EA (zmieniany jest jeden parametr naraz):
`pop=100 gen=100 Px=0.7 Pm=0.1 Tour=5 elite=1 OX inwersja, start losowy, pakowanie simple`.

## 1. Wpływ parametrów EA na różnych instancjach

### Mutacja swap, Pm na gen

| Pm | easy1 | medium1 | hard1 | extra |
|---|---:|---:|---:|---:|
| 0.0 | 3 997 ± 2 357 | 393 734 ± 4 976 | 1 310 307 ± 3 032 | 218 409 ± 3 095 |
| 0.001 | 14 531 ± 1 080 | 413 663 ± 1 856 | **1 324 507** ± 1 204 | **235 958** ± 1 949 |
| 0.005 | **15 084** ± 819 | **413 748** ± 2 178 | 1 324 500 ± 2 054 | 235 865 ± 1 302 |
| 0.01 | 12 028 ± 1 047 | 407 549 ± 1 809 | 1 319 624 ± 1 754 | 231 739 ± 1 882 |
| 0.05 | 4 443 ± 498 | 391 013 ± 2 567 | 1 307 244 ± 1 877 | 218 538 ± 1 417 |

Wszędzie ten sam kształt: bez mutacji i przy zbyt dużej mutacji wyniki są najgorsze, optimum leży przy Pm ≈ 0.001-0.005.

### Rozmiar turnieju (Tour)

| Tour | easy1 | medium1 | hard1 | extra |
|---|---:|---:|---:|---:|
| 1 | -1 643 ± 779 | 378 023 ± 1 229 | 1 298 074 ± 1 497 | 206 831 ± 1 613 |
| 2 | 10 392 ± 1 327 | **404 449** ± 1 960 | 1 319 605 ± 1 641 | **228 156** ± 1 991 |
| 5 | 11 275 ± 1 211 | 402 035 ± 3 468 | **1 325 077** ± 1 294 | 226 396 ± 4 015 |
| 10 | **12 408** ± 1 121 | 399 985 ± 2 470 | 1 324 068 ± 1 474 | 225 489 ± 3 025 |
| 20 | 11 107 ± 828 | 397 295 ± 3 309 | 1 322 591 ± 1 328 | 223 637 ± 2 037 |
| 100 | 11 204 ± 1 371 | 396 007 ± 2 781 | 1 323 033 ± 1 297 | 221 582 ± 2 376 |

Tour = 1 to losowy wybór rodzica (brak presji selekcyjnej), wyraźnie najgorzej. Dla Tour ≥ 2 różnice są małe; bardzo duży Tour lekko pogarsza wynik (szybka utrata różnorodności).

### Prawdopodobieństwo krzyżowania (Px)

| Px | easy1 | medium1 | hard1 | extra |
|---|---:|---:|---:|---:|
| 0.0 | 1 729 ± 1 338 | 384 354 ± 1 874 | 1 305 720 ± 1 894 | 212 082 ± 1 913 |
| 0.5 | 9 817 ± 846 | 396 147 ± 3 431 | 1 320 762 ± 1 804 | 221 868 ± 938 |
| 0.7 | 11 275 ± 1 211 | 402 035 ± 3 468 | 1 325 077 ± 1 294 | 226 396 ± 4 015 |
| 1.0 | **13 947** ± 1 326 | **408 301** ± 2 463 | **1 327 348** ± 1 980 | **231 519** ± 2 137 |

Krzyżowanie jest kluczowe: bez niego EA to praktycznie sama mutacja z selekcją. Uwaga: przy Px < 1 część dzieci to kopie rodziców, które nie są oceniane ponownie, więc mniejsze Px = też mniej ocen.

### Selekcja: turniej vs ruletka

| | easy1 | medium1 | hard1 | extra |
|---|---:|---:|---:|---:|
| turniej (Tour=5) | **11 275** | **402 035** | **1 325 077** | **226 396** |
| ruletka | 7 541 | 394 723 | 1 312 954 | 220 616 |

Ruletka przegrywa na każdej instancji (patrz pytanie 3).

### Ten sam budżet ocen: populacja vs pokolenia (pop × gen = 10 000)

| pop × gen | easy1 | medium1 | hard1 | extra |
|---|---:|---:|---:|---:|
| 20 × 500 | 10 129 | 391 161 | 1 320 775 | 219 981 |
| 50 × 200 | **11 809** | 397 878 | 1 324 964 | 223 076 |
| 100 × 100 | 11 275 | 402 035 | **1 325 077** | 226 396 |
| 200 × 50 | 10 855 | **404 570** | 1 320 415 | **228 075** |
| 500 × 20 | 6 500 | 395 645 | 1 312 656 | 219 984 |

Najlepiej wypada środek (pop 50-200). Za mała populacja traci różnorodność, za duża nie zdąży ewoluować.

### Inicjalizacja zachłanna (część populacji z najbliższego sąsiada)

| greedyInit | easy1 | medium1 | hard1 | extra |
|---|---:|---:|---:|---:|
| 0.0 | 11 275 | 402 035 | 1 325 077 | 226 396 |
| 0.1 | 35 048 | 456 104 | 1 362 032 | 270 402 |
| 0.5 | 35 932 | 455 917 | **1 362 614** | 271 328 |
| 1.0 | **36 080** | **456 436** | 1 362 521 | **271 390** |
| *zachłanny best* | *35 071* | *446 832* | *1 363 918* | *267 553* |

Największy efekt ze wszystkich parametrów. Już 10% tras zachłannych wystarcza, a EA poprawia je dalej.

## 2. Pakowanie z kontrolą opłacalności ("smart")

Pomysł: brać przedmiot tylko, gdy jego zysk przewyższa szacowany dodatkowy czas jazdy.

| Metoda | instancja | simple | smart |
|---|---|---:|---:|
| zachłanny (avg) | easy1 | **31 505** | 30 525 |
| zachłanny (avg) | easy2 | **28 454** | 27 493 |
| zachłanny (avg) | hard1 | **1 358 602** | 1 358 290 |
| zachłanny (avg) | medium1, medium2, hard2, extra | bez zmian | bez zmian |
| EA dostrojony (avg) | extra | **271 683** | 270 052 |

**Wynik negatywny:** proste oszacowanie jest zbyt pesymistyczne. Liczy spowolnienie przy całej wadze już wybranych przedmiotów, a te zwykle leżą dalej na trasie i na początku odcinka jeszcze ich nie ma. Przez to odrzuca przedmioty, które się opłacają. Dobra kontrola opłacalności wymagałaby symulacji trasy dla każdego przedmiotu, co jest wielokrotnie droższe.

## 3. Finalne porównanie metod (7 instancji, budżet 10 000 ocen)

Konfiguracje:
- **Losowy:** 10 000 losowych tras,
- **Zachłanny:** najbliższy sąsiad z każdego z 280 miast (280 ocen),
- **EA start:** parametry z treści `pop=100 gen=100 Px=0.7 Pm=0.1 Tour=5`, inwersja, start losowy,
- **SA start:** `iter=10000 T0=auto cooling=0.999`, inwersja, start losowy,
- **EA dostrojony:** `pop=100 Px=0.7 Tour=5 elite=1 OX`, swap `Pm=0.002`, `greedyInit=0.5`, limit 10 000 ocen,
- **SA dostrojony:** `iter=9999 T0=10 cooling=0.999`, ruch swap, start zachłanny.

| instancja | Losowy | Zachłanny best / avg | EA start | SA start | **EA dostrojony** | **SA dostrojony** |
|---|---:|---:|---:|---:|---:|---:|
| easy1 | -10 849 | 35 071 / 31 505 | 11 275 | 24 765 | **35 883** ± 506 | 34 354 ± 1 386 |
| easy2 | -7 290 | 30 882 / 28 454 | 12 399 | 23 719 | **31 823** ± 420 | 30 916 ± 958 |
| medium1 | 358 514 | 446 832 / 437 465 | 402 035 | 418 907 | **457 212** ± 908 | 456 180 ± 1 374 |
| medium2 | 685 295 | 745 530 / 739 152 | 715 048 | 728 867 | **753 236** ± 1 017 | 751 549 ± 1 214 |
| hard1 | 1 283 352 | 1 363 918 / 1 358 602 | 1 325 077 | 1 344 272 | **1 361 980** ± 848 | 1 360 805 ± 684 |
| hard2 | 2 844 927 | 2 921 166 / 2 918 357 | 2 882 361 | 2 899 193 | **2 921 840** ± 513 | 2 919 716 ± 938 |
| extra | 191 332 | 267 553 / 260 597 | 226 396 | 243 523 | **271 683** ± 853 | 269 882 ± 778 |

(dla EA/SA podana średnia; pełna tabela best/worst/avg/std: `docs/wyniki/tabela.md`)

- Po strojeniu EA i SA mają średnią wyższą niż średnia zachłannego na wszystkich instancjach, a EA także wyższą niż najlepszy wynik zachłannego na 6 z 7 (poza hard1, gdzie jest o 0.1% niżej).
- EA dostrojony ma najwyższą średnią na każdej instancji i zwykle najmniejsze std (5 z 7 instancji), czyli jest też najstabilniejszy.
- Przed strojeniem kolejność była: zachłanny > SA > EA > losowy.

## 4. Odpowiedzi na pytania pomocnicze (szkic)

1. **Jak Tour wpływa na EA? Tour=1 i Tour=pop_size?** Tour steruje presją selekcyjną. Tour=1 to losowy wybór rodziców, brak presji, więc EA błądzi (najgorsze wyniki na każdej instancji). Tour=pop_size prawie zawsze wybiera najlepszego osobnika, populacja szybko się ujednolica i utyka; w danych widać lekki spadek dla Tour=100.
2. **Czym różni się Tour=1 od Tour>1?** Przy Tour=1 o tym, kto się rozmnaża, decyduje przypadek, a nie jakość, więc poprawa wynika tylko z elitaryzmu i szczęścia. Już Tour=2 daje duży skok (np. extra: 206 831 → 228 156, medium1: 378 023 → 404 449).
3. **Czy ruletka działa poprawnie dla TTP?** Słabo. G bywa ujemne, więc trzeba je przesuwać (u nas: G - G_najgorszego), a przy dużych wartościach G (setki tysięcy, miliony) różnice między osobnikami to ułamki procenta. Prawdopodobieństwa wyboru są prawie równe, presja selekcyjna znika. Ruletka przegrywa z turniejem na każdej instancji; pomogłoby skalowanie przystosowania.
4. **Czy mutacji może być za mało lub za dużo?** Tak, oba końce są złe. Pm=0 i Pm=0.05 (swap na gen) dają wyniki najgorsze i prawie równe; optimum leży przy 0.001-0.005.
5. **Jaką rolę pełni mutacja?** Wprowadza nowe kombinacje i utrzymuje różnorodność populacji, pozwala wyjść z lokalnego optimum i naprawia trasy, których krzyżowanie samo nie poprawi.
6. **Czy krzyżowania może być za mało lub za dużo?** Za mało na pewno: Px=0 jest wyraźnie najgorsze. W naszych testach wynik rośnie aż do Px=1.0, więc "za dużo" nie wystąpiło (mutacja i elitaryzm utrzymują różnorodność).
7. **Jaką rolę pełni krzyżowanie?** Łączy dobre fragmenty tras dwóch rodziców (OX zachowuje fragment i względną kolejność miast). To główny motor poprawy w EA.
8. **Co się dzieje bez krzyżowania i/lub mutacji?** Bez krzyżowania EA zamienia się w powolne przeszukiwanie mutacją (Px=0 najgorsze). Bez mutacji populacja szybko traci różnorodność i stoi w miejscu (Pm=0). Bez obu zostaje tylko selekcja istniejących osobników, czyli brak postępu poza najlepszym osobnikiem startowym.
9. **Czy selekcja elitarna jest potrzebna?** Przy pop=100 i starcie losowym wpływ jest znikomy (elite 0-10 w granicach std). Przy starcie zachłannym elitaryzm pomaga: bez niego (elite=0) najlepsza trasa zachłanna może zginąć (extra, EA ze startem zachłannym i swap Pm=0.005: 270 386 z elite=1 vs 268 079 z elite=0).
