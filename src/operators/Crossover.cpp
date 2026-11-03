#include "ttp/operators/Crossover.h"

std::vector<int> orderCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2, int a, int b) {
    int n = static_cast<int>(parent1.size());

    std::vector<int> child(n, -1);
    std::vector<bool> used(n, false);

    // fragment z rodzica 1
    for (int i = a; i <= b; i++) {
        child[i] = parent1[i];
        used[parent1[i]] = true;
    }

    // reszta z rodzica 2 po kolei
    int pos = 0;
    for (int k = 0; k < n; k++) {
        int city = parent2[k];
        if (used[city]) {
            continue;
        }

        while (child[pos] != -1) {
            pos++;
        }

        child[pos] = city;
        used[city] = true;
    }

    return child;
}

std::vector<int> pmxCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2, int a, int b) {
    int n = static_cast<int>(parent1.size());

    std::vector<int> child(n, -1);
    std::vector<bool> inSegment(n, false);
    std::vector<int> positionInParent1(n);

    for (int i = 0; i < n; i++) {
        positionInParent1[parent1[i]] = i;
    }

    // fragment z rodzica 1
    for (int i = a; i <= b; i++) {
        child[i] = parent1[i];
        inSegment[parent1[i]] = true;
    }

    // reszta z rodzica 2
    for (int i = 0; i < n; i++) {
        if (i >= a && i <= b) {
            continue;
        }

        int city = parent2[i];

        // jak miasto juz jest we fragmencie to idziemy po mapowaniu
        while (inSegment[city]) {
            int pos = positionInParent1[city];
            city = parent2[pos];
        }

        child[i] = city;
    }

    return child;
}

static void randomCutPoints(int n, Rng& rng, int& a, int& b) {
    a = rng.uniformInt(0, n - 1);
    b = rng.uniformInt(0, n - 1);
    if (a > b) {
        int temp = a;
        a = b;
        b = temp;
    }
}

Individual crossoverOX(const Individual& parent1, const Individual& parent2, Rng& rng) {
    int n = static_cast<int>(parent1.route.size());
    int a = 0;
    int b = 0;
    randomCutPoints(n, rng, a, b);

    Individual child;
    child.route = orderCrossover(parent1.route, parent2.route, a, b);
    child.picked.assign(parent1.picked.size(), 0);
    child.evaluated = false;
    return child;
}

Individual crossoverPMX(const Individual& parent1, const Individual& parent2, Rng& rng) {
    int n = static_cast<int>(parent1.route.size());
    int a = 0;
    int b = 0;
    randomCutPoints(n, rng, a, b);

    Individual child;
    child.route = pmxCrossover(parent1.route, parent2.route, a, b);
    child.picked.assign(parent1.picked.size(), 0);
    child.evaluated = false;
    return child;
}
