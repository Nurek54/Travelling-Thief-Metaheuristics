#pragma once

#include <string>
#include <vector>

#include "ttp/solvers/EvolutionaryAlgorithm.h"
#include "ttp/solvers/SimulatedAnnealing.h"

void saveHistoryCsv(const std::string& path, const std::vector<GenerationStats>& history);

void saveSAHistoryCsv(const std::string& path, const std::vector<SAStats>& history);
