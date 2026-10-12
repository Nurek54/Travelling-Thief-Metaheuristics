// Wspolny wynik dla wszystkich metod.
#pragma once

#include <vector>

#include "ttp/Individual.h"

struct SolverResult {
    std::vector<double> values;  // G kazdego ocenionego rozwiazania (do statystyk)
    Individual best;             // najlepsze znalezione rozwiazanie
};
