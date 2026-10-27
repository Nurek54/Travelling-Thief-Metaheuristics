#pragma once

#include <fstream>
#include <string>

#include "ttp/Config.h"
#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Problem.h"
#include "ttp/solvers/Solver.h"

void printResult(std::ofstream& csv, const std::string& instance, const std::string& method,
                 const SolverResult& result, long long evaluations, double timeMs);

void runEaSeries(std::ofstream& csv, const std::string& instance, const std::string& methodName,
                 const Problem& problem, const Distance& distance, Evaluator& evaluator,
                 const EAConfig& config, int runs, unsigned int seed, const std::string& historyFolder);

void runSaSeries(std::ofstream& csv, const std::string& instance, const std::string& methodName,
                 const Problem& problem, const Distance& distance, Evaluator& evaluator,
                 const SAConfig& config, int runs, unsigned int seed, const std::string& historyFolder);

int runFullComparison();
