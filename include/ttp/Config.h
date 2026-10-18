// Parametry algorytmu ewolucyjnego: pop_size, gen, Px, Pm, Tour, elita, wybor operatorow.
#pragma once

#include <string>

enum class SelectionType {
    Tournament,
    Roulette
};

enum class CrossoverType {
    OX,
    PMX
};

enum class MutationType {
    Swap,       // Pm na gen (start od 0.01)
    Inversion   // Pm na osobnika (start od 0.1)
};

struct EAConfig {
    // Wartosci startowe z tresci: pop_size=100, gen=100, Px=0.7, Pm=0.1, Tour=5
    int popSize = 100;
    int generations = 100;
    double px = 0.7;   // prawdopodobienstwo krzyzowania
    double pm = 0.1;   // prawdopodobienstwo mutacji
    int tourSize = 5;  // rozmiar turnieju
    int elitism = 1;   // ilu najlepszych przechodzi bez zmian do nastepnego pokolenia

    SelectionType selection = SelectionType::Tournament;
    CrossoverType crossover = CrossoverType::OX;
    MutationType mutation = MutationType::Inversion;
};

// Parametry symulowanego wyzarzania (SA).
struct SAConfig {
    int iterations = 10000;          // ile sasiadow sprawdzamy (tyle ocen co EA/losowy, uczciwe porownanie)
    double startTemperature = 0.0;   // 0 = dobierz automatycznie na podstawie instancji
    double coolingRate = 0.999;      // po kazdej iteracji: T = T * coolingRate
    MutationType move = MutationType::Inversion;  // ruch do sasiada: inwersja albo swap
    int logEvery = 100;              // co ile iteracji zapisac wiersz do CSV
};

// Opis konfiguracji jednym tekstem, np. do wypisania lub do nazwy pliku z wynikami.
std::string describeConfig(const EAConfig& config);
std::string describeConfig(const SAConfig& config);
