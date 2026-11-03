#pragma once

#include <vector>

#include "ttp/Individual.h"
#include "ttp/Rng.h"

void swapCities(std::vector<int>& route, int i, int j);

void reverseSegment(std::vector<int>& route, int i, int j);

// swap pm liczone na gen
void mutateSwap(Individual& individual, double pm, Rng& rng);

// inwersja pm na osobnika max jedno odwrocenie
void mutateInversion(Individual& individual, double pm, Rng& rng);
