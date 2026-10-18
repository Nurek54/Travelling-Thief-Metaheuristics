// Zapis przebiegu algorytmu do CSV: pokolenie;best;avg;worst (format z tresci).
#pragma once

#include <string>
#include <vector>

#include "ttp/solvers/EvolutionaryAlgorithm.h"
#include "ttp/solvers/SimulatedAnnealing.h"

// Zapisuje historie pokolen do pliku CSV. Tworzy brakujace foldery.
// Rzuca std::runtime_error, jesli pliku nie da sie utworzyc.
void saveHistoryCsv(const std::string& path, const std::vector<GenerationStats>& history);

// Historia SA: iteration;best;current;temperature
void saveSAHistoryCsv(const std::string& path, const std::vector<SAStats>& history);
