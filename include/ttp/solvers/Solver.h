#pragma once

#include <vector>

#include "ttp/Individual.h"

struct SolverResult {
    std::vector<double> values;
    Individual best;
};
