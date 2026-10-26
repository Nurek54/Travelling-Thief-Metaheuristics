#include "ttp/solvers/RandomSearch.h"

SolverResult runRandomSearch(const Problem& problem, Evaluator& evaluator, Rng& rng, int count) {
    SolverResult result;

    for (int i = 0; i < count; i++) {
        Individual individual = makeRandomIndividual(problem, rng);
        double G = evaluator.evaluate(individual);
        result.values.push_back(G);

        // Pierwszy osobnik albo lepszy od dotychczasowego najlepszego
        if (i == 0 || G > result.best.fitness) {
            result.best = individual;
        }
    }

    return result;
}
