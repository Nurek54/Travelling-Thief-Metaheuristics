#include "ttp/operators/Mutation.h"

void swapCities(std::vector<int>& route, int i, int j) {
    int temp = route[i];
    route[i] = route[j];
    route[j] = temp;
}

void reverseSegment(std::vector<int>& route, int i, int j) {
    while (i < j) {
        swapCities(route, i, j);
        i++;
        j--;
    }
}

void mutateSwap(Individual& individual, double pm, Rng& rng) {
    int n = static_cast<int>(individual.route.size());
    bool changed = false;

    for (int i = 0; i < n; i++) {
        if (rng.uniformReal() < pm) {
            int j = rng.uniformInt(0, n - 1);
            swapCities(individual.route, i, j);
            changed = true;
        }
    }

    if (changed) {
        individual.evaluated = false;
    }
}

void mutateInversion(Individual& individual, double pm, Rng& rng) {
    if (rng.uniformReal() >= pm) {
        return;
    }

    int n = static_cast<int>(individual.route.size());
    int i = rng.uniformInt(0, n - 1);
    int j = rng.uniformInt(0, n - 1);

    if (i > j) {
        int temp = i;
        i = j;
        j = temp;
    }

    reverseSegment(individual.route, i, j);
    individual.evaluated = false;
}
