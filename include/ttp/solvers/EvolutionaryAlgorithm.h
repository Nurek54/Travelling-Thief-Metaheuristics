#pragma once

#include <vector>

#include "ttp/Config.h"
#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"
#include "ttp/Rng.h"

struct GenerationStats {
    int generation = 0;
    double best = 0.0;
    double avg = 0.0;
    double worst = 0.0;
};

struct EAResult {
    Individual best;
    std::vector<GenerationStats> history;
};

EAResult runEvolutionaryAlgorithm(const Problem& problem, const Distance& distance, Evaluator& evaluator,
                                  const EAConfig& config, Rng& rng);
