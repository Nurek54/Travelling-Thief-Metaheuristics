#pragma once

#include <vector>

#include "ttp/Individual.h"
#include "ttp/Rng.h"

// turniej tour=1 to po prostu losowy wybor
int tournamentSelection(const std::vector<Individual>& population, int tourSize, Rng& rng);

// ruletka G bywa ujemne wiec odejmujemy najgorsze
int rouletteSelection(const std::vector<Individual>& population, Rng& rng);
