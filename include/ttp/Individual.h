#pragma once

#include <vector>

#include "ttp/Problem.h"
#include "ttp/Rng.h"

struct Individual {
    std::vector<int> route;

    std::vector<char> picked;

    double fitness = 0.0;
    bool evaluated = false;

    long long totalProfit = 0;
    int totalWeight = 0;
    double travelTime = 0.0;
};

Individual makeIdentityIndividual(const Problem& problem);

Individual makeRandomIndividual(const Problem& problem, Rng& rng);

bool isValidRoute(const Individual& individual, int numCities);
