// Implementacja Distance.h
#include "ttp/Distance.h"

#include <cmath>

Distance::Distance(const Problem& problem)
    : numCities_(problem.numCities),
      matrix_(numCities_ * numCities_, 0) {
    for (int i = 0; i < numCities_; ++i) {
        const City& a = problem.cities[i];

        for (int j = 0; j < numCities_; ++j) {
            const City& b = problem.cities[j];

            const double dx = a.x - b.x;
            const double dy = a.y - b.y;
            const double euclidean = std::sqrt(dx * dx + dy * dy);

            matrix_[i * numCities_ + j] = static_cast<int>(std::ceil(euclidean));
        }
    }
}

int Distance::get(int from, int to) const {
    return matrix_[from * numCities_ + to];
}
