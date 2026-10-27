#include "ttp/Stats.h"

#include <cmath>

Stats computeStats(const std::vector<double>& values) {
    Stats stats;
    if (values.empty()) {
        return stats;
    }

    stats.best = values[0];
    stats.worst = values[0];
    double sum = 0.0;
    for (double value : values) {
        if (value > stats.best) {
            stats.best = value;
        }
        if (value < stats.worst) {
            stats.worst = value;
        }
        sum += value;
    }
    stats.avg = sum / values.size();

    double sumSquares = 0.0;
    for (double value : values) {
        double diff = value - stats.avg;
        sumSquares += diff * diff;
    }
    stats.std = std::sqrt(sumSquares / values.size());

    return stats;
}
