#pragma once

#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Problem.h"
#include "ttp/solvers/Solver.h"

// najblizszy sasiad od startCity
Individual makeNearestNeighbourIndividual(const Problem& problem, const Distance& distance, int startCity);

SolverResult runGreedy(const Problem& problem, const Distance& distance, Evaluator& evaluator);
