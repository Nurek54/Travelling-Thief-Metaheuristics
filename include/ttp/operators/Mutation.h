#pragma once

#include <vector>

#include "ttp/Individual.h"
#include "ttp/Rng.h"

void swapCities(std::vector<int>& route, int i, int j);

void reverseSegment(std::vector<int>& route, int i, int j);

// Swap: Pm na gen.
// Dla kazdej pozycji z prawdopodobienstwem pm zamieniamy ja z losowa inna pozycja.
void mutateSwap(Individual& individual, double pm, Rng& rng);

// Inwersja: Pm na osobnika.
// Z prawdopodobienstwem pm odwracamy jeden losowy fragment trasy.
void mutateInversion(Individual& individual, double pm, Rng& rng);
