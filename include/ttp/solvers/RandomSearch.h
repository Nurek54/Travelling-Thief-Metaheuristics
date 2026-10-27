#pragma once

#include "ttp/Evaluator.h"
#include "ttp/Problem.h"
#include "ttp/Rng.h"
#include "ttp/solvers/Solver.h"

SolverResult runRandomSearch(const Problem& problem, Evaluator& evaluator, Rng& rng, int count);
