// Odleglosc ceil(euklidesowa) miedzy miastami, liczona raz na starcie.
#pragma once

#include <vector>

#include "ttp/Problem.h"

class Distance {
public:
    explicit Distance(const Problem& problem);

    int get(int from, int to) const;

private:
    int numCities_ = 0;
    std::vector<int> matrix_;  // plaska macierz n*n, element (i, j) pod indeksem i * n + j
};