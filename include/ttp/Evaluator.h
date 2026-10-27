#pragma once

#include "ttp/Distance.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"

class Evaluator {
public:
    Evaluator(const Problem& problem, const Distance& distance, bool smartPacking = false);

    double evaluate(Individual& individual);

    long long getCount() const;
    void resetCount();

private:
    const Problem& problem_;
    const Distance& distance_;
    bool smartPacking_ = false;
    long long count_ = 0;
};
