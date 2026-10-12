// Statystyki z wielu wynikow: best, worst, avg, std.
#pragma once

#include <vector>

struct Stats {
    double best = 0.0;   // najwieksze G (maksymalizujemy)
    double worst = 0.0;  // najmniejsze G
    double avg = 0.0;    // srednia
    double std = 0.0;    // odchylenie standardowe
};

Stats computeStats(const std::vector<double>& values);
