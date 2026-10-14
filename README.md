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
- [ ] Implementacja narzędzi logowania → wykresy
- [ ] Implementacja innej metaheurystyki (SA lub TS)
- [ ] Wstępne strojenie metaheurystyk
- [ ] Badanie wpływu wartości parametrów na skuteczność/efektywność algorytmu genetycznego
- [ ] Przygotowanie sprawozdania: tabelki, wykresy, wnioski

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
