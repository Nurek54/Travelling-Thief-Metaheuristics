#pragma once

#include <vector>

#include "ttp/Config.h"
#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"
#include "ttp/Rng.h"

struct SAStats {
    int iteration = 0;
    double best = 0.0;
    double current = 0.0;
    double temperature = 0.0;
};

struct SAResult {
    Individual best;
    std::vector<SAStats> history;
    double startTemperature = 0.0;
};

SAResult runSimulatedAnnealing(const Problem& problem, const Distance& distance, Evaluator& evaluator,
                               const SAConfig& config, Rng& rng);
