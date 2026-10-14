// Algorytm ewolucyjny: populacja, selekcja, krzyzowanie, mutacja, elitaryzm.
#pragma once

#include <vector>

#include "ttp/Config.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"
#include "ttp/Rng.h"

// Statystyki jednego pokolenia (do logow i wykresow).
struct GenerationStats {
    int generation = 0;
    double best = 0.0;
    double avg = 0.0;
    double worst = 0.0;
};

struct EAResult {
    Individual best;                       // najlepszy osobnik z calego przebiegu
    std::vector<GenerationStats> history;  // statystyki kazdego pokolenia (0 = populacja startowa)
};

EAResult runEvolutionaryAlgorithm(const Problem& problem, Evaluator& evaluator, const EAConfig& config, Rng& rng);
