// Osobnik: trasa (permutacja miast), plan przedmiotow, fitness.
#pragma once

#include <vector>

#include "ttp/Problem.h"
#include "ttp/Rng.h"

struct Individual {
    // Kolejnosc odwiedzania miast. route[0] to miasto startowe, na koncu wracamy do niego.
    // Kazde miasto wystepuje dokladnie raz.
    std::vector<int> route;

    // picked[k] == 1 oznacza, ze przedmiot k jest zabierany. Wypelnia PackingHeuristic.
    // char zamiast bool, bo std::vector<bool> jest specjalnym, wolniejszym przypadkiem.
    std::vector<char> picked;

    // Wynik funkcji oceny G = zysk - czas. Wazny tylko, gdy evaluated == true.
    double fitness = 0.0;
    bool evaluated = false;

    // Dodatkowe informacje z ostatniej oceny (do debugowania i logow).
    long long totalProfit = 0;
    int totalWeight = 0;
    double travelTime = 0.0;
};

// Osobnik z trasa 0, 1, 2, ..., n-1 i pustym planem przedmiotow.
Individual makeIdentityIndividual(const Problem& problem);

// Osobnik z losowa trasa (losowa permutacja wszystkich miast) i pustym planem przedmiotow.
Individual makeRandomIndividual(const Problem& problem, Rng& rng);

// Czy trasa jest poprawna permutacja miast 0..n-1 (kazde dokladnie raz).
// Przyda sie do testowania krzyzowania i mutacji.
bool isValidRoute(const Individual& individual, int numCities);
