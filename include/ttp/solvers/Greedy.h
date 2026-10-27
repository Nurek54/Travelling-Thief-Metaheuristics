#pragma once

#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Problem.h"
#include "ttp/solvers/Solver.h"

// Trasa "najblizszego sasiada": z miasta startCity zawsze idziemy do najblizszego nieodwiedzonego.
Individual makeNearestNeighbourIndividual(const Problem& problem, const Distance& distance, int startCity);

SolverResult runGreedy(const Problem& problem, const Distance& distance, Evaluator& evaluator);
