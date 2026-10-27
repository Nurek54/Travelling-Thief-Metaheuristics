#pragma once

#include <vector>

struct Stats {
    double best = 0.0;
    double worst = 0.0;
    double avg = 0.0;
    double std = 0.0;
};

Stats computeStats(const std::vector<double>& values);
