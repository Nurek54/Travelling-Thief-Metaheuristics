#pragma once

#include <vector>

#include "ttp/Problem.h"

class Distance {
public:
    explicit Distance(const Problem& problem);

    int get(int from, int to) const;

private:
    int numCities_ = 0;
    std::vector<int> matrix_;
};