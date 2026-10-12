// Implementacja Individual.h
#include "ttp/Individual.h"

#include <algorithm>
#include <numeric>

Individual makeIdentityIndividual(const Problem& problem) {
    Individual individual;
    individual.route.resize(problem.numCities);
    std::iota(individual.route.begin(), individual.route.end(), 0); // 0, 1, 2, ..., n-1
    individual.picked.assign(problem.numItems, 0);
    return individual;
}

Individual makeRandomIndividual(const Problem& problem, Rng& rng) {
    Individual individual = makeIdentityIndividual(problem);
    std::shuffle(individual.route.begin(), individual.route.end(), rng.engine());
    return individual;
}

bool isValidRoute(const Individual& individual, int numCities) {
    if (static_cast<int>(individual.route.size()) != numCities) {
        return false;
    }

    std::vector<char> seen(numCities, 0);
    for (const int city : individual.route) {
        if (city < 0 || city >= numCities || seen[city]) {
            return false; // miasto spoza zakresu albo powtorzone
        }
        seen[city] = 1;
    }
    return true;
}
