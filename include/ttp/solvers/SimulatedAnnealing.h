// Symulowane wyzarzanie (SA): jedno rozwiazanie, ktore krok po kroku zmieniamy na sasiada.
#pragma once

#include <vector>

#include "ttp/Config.h"
#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"
#include "ttp/Rng.h"

// Jeden wiersz historii SA (do CSV i wykresow).
struct SAStats {
    int iteration = 0;
    double best = 0.0;         // najlepsze G znalezione do tej pory
    double current = 0.0;      // G aktualnego rozwiazania
    double temperature = 0.0;  // aktualna temperatura
};

struct SAResult {
    Individual best;
    std::vector<SAStats> history;
    double startTemperature = 0.0;  // uzyta temperatura poczatkowa (przydatne, gdy byla dobrana automatycznie)
};

// distance jest potrzebny tylko do startu zachlannego (greedyStart = true)
SAResult runSimulatedAnnealing(const Problem& problem, const Distance& distance, Evaluator& evaluator,
                               const SAConfig& config, Rng& rng);
