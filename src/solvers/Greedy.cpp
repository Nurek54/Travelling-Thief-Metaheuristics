// Implementacja Greedy.h
#include "ttp/solvers/Greedy.h"

#include <vector>

Individual makeNearestNeighbourIndividual(const Problem& problem, const Distance& distance, int startCity) {
    int n = problem.numCities;

    Individual individual;
    individual.picked.assign(problem.numItems, 0);

    std::vector<bool> visited(n, false);
    int current = startCity;
    individual.route.push_back(current);
    visited[current] = true;

    // Dokladamy kolejne n - 1 miast
    for (int step = 1; step < n; step++) {
        int nearest = -1;
        int nearestDistance = 0;

        // Szukamy najblizszego nieodwiedzonego miasta
        for (int city = 0; city < n; city++) {
            if (visited[city]) {
                continue;
            }
            int d = distance.get(current, city);
            if (nearest == -1 || d < nearestDistance) {
                nearest = city;
                nearestDistance = d;
            }
        }

        individual.route.push_back(nearest);
        visited[nearest] = true;
        current = nearest;
    }

    return individual;
}

SolverResult runGreedy(const Problem& problem, const Distance& distance, Evaluator& evaluator) {
    SolverResult result;

    for (int start = 0; start < problem.numCities; start++) {
        Individual individual = makeNearestNeighbourIndividual(problem, distance, start);
        double G = evaluator.evaluate(individual);
        result.values.push_back(G);

        if (start == 0 || G > result.best.fitness) {
            result.best = individual;
        }
    }

    return result;
}
