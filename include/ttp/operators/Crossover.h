#pragma once

#include <vector>

#include "ttp/Individual.h"
#include "ttp/Rng.h"

// OX fragment a..b z rodzica 1 reszta w kolejnosci z rodzica 2
std::vector<int> orderCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2, int a, int b);

// PMX fragment a..b z rodzica 1 reszta z rodzica 2 duplikaty przez mapowanie
std::vector<int> pmxCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2, int a, int b);

Individual crossoverOX(const Individual& parent1, const Individual& parent2, Rng& rng);
Individual crossoverPMX(const Individual& parent1, const Individual& parent2, Rng& rng);
