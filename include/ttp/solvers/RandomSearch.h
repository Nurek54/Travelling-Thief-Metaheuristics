// Metoda losowa: generujemy wiele losowych osobnikow i zapamietujemy najlepszego.
#pragma once

#include "ttp/Evaluator.h"
#include "ttp/Problem.h"
#include "ttp/Rng.h"
#include "ttp/solvers/Solver.h"

// count: ile losowych osobnikow wygenerowac (w tresci 10 000).
SolverResult runRandomSearch(const Problem& problem, Evaluator& evaluator, Rng& rng, int count);
