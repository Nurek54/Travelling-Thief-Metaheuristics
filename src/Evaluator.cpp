#include "ttp/Evaluator.h"

#include "ttp/PackingHeuristic.h"

Evaluator::Evaluator(const Problem& problem, const Distance& distance, bool smartPacking)
    : problem_(problem), distance_(distance), smartPacking_(smartPacking) {}

double Evaluator::evaluate(Individual& individual) {
    count_++;

    packItems(problem_, distance_, individual, smartPacking_);

    int n = problem_.numCities;
    double vmax = problem_.maxSpeed;
    double vmin = problem_.minSpeed;
    double W = problem_.capacity;

    int currentWeight = 0;
    long long profit = 0;
    double time = 0.0;

    for (int pos = 1; pos < n; pos++) {
        int from = individual.route[pos - 1];
        int to = individual.route[pos];

        double v = vmax - currentWeight * (vmax - vmin) / W;
        time += distance_.get(from, to) / v;

        for (int k : problem_.itemsByCity[to]) {
            if (individual.picked[k] == 1) {
                currentWeight += problem_.items[k].weight;
                profit += problem_.items[k].profit;
            }
        }
    }

    int lastCity = individual.route[n - 1];
    int startCity = individual.route[0];
    double v = vmax - currentWeight * (vmax - vmin) / W;
    time += distance_.get(lastCity, startCity) / v;

    double G = profit - time;

    individual.fitness = G;
    individual.evaluated = true;
    individual.totalProfit = profit;
    individual.totalWeight = currentWeight;
    individual.travelTime = time;

    return G;
}

long long Evaluator::getCount() const {
    return count_;
}

void Evaluator::resetCount() {
    count_ = 0;
}
