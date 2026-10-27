#include "ttp/PackingHeuristic.h"

#include <algorithm>
#include <utility>
#include <vector>

void packItems(const Problem& problem, const Distance& distance, Individual& individual, bool profitCheck) {
    int n = problem.numCities;
    int m = problem.numItems;

    std::vector<int> positionInRoute(n);
    for (int pos = 0; pos < n; pos++) {
        int city = individual.route[pos];
        positionInRoute[city] = pos;
    }

    std::vector<double> remainingDistance(n);
    int lastCity = individual.route[n - 1];
    int startCity = individual.route[0];
    remainingDistance[n - 1] = distance.get(lastCity, startCity);
    for (int pos = n - 2; pos >= 0; pos--) {
        int cityHere = individual.route[pos];
        int cityNext = individual.route[pos + 1];
        remainingDistance[pos] = remainingDistance[pos + 1] + distance.get(cityHere, cityNext);
    }

    std::vector<std::pair<double, int>> scores;
    for (int k = 0; k < m; k++) {
        const Item& item = problem.items[k];
        int pos = positionInRoute[item.city];

        if (pos == 0) {
            continue;
        }

        double dist = remainingDistance[pos];
        if (dist < 1.0) {
            dist = 1.0;
        }

        double score = item.profit / (item.weight * dist);
        scores.push_back(std::make_pair(score, k));
    }

    std::sort(scores.begin(), scores.end());

    individual.picked.assign(m, 0);
    int currentWeight = 0;
    double vmax = problem.maxSpeed;
    double vmin = problem.minSpeed;
    double W = problem.capacity;

    for (int i = static_cast<int>(scores.size()) - 1; i >= 0; i--) {
        int k = scores[i].second;
        int weight = problem.items[k].weight;

        if (currentWeight + weight > problem.capacity) {
            continue;
        }

        if (profitCheck) {
            double dist = remainingDistance[positionInRoute[problem.items[k].city]];
            double speedBefore = vmax - currentWeight * (vmax - vmin) / W;
            double speedAfter = vmax - (currentWeight + weight) * (vmax - vmin) / W;
            double extraTime = dist / speedAfter - dist / speedBefore;

            if (problem.items[k].profit <= extraTime) {
                continue;
            }
        }

        individual.picked[k] = 1;
        currentWeight += weight;
    }
}
