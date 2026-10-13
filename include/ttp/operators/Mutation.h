// Mutacja: Swap, Inversion.
#pragma once

#include <vector>

#include "ttp/Individual.h"
#include "ttp/Rng.h"

// Zamienia miejscami miasta na pozycjach i oraz j.
void swapCities(std::vector<int>& route, int i, int j);

// Odwraca kolejnosc miast od pozycji i do j (wlacznie).
void reverseSegment(std::vector<int>& route, int i, int j);

// SWAP: Pm liczone na GEN.
// Dla kazdej pozycji z prawdopodobienstwem pm zamieniamy ja z losowa inna pozycja.
// Tresc sugeruje start od pm = 0.01.
void mutateSwap(Individual& individual, double pm, Rng& rng);

// INWERSJA: Pm liczone na OSOBNIKA.
// Z prawdopodobienstwem pm odwracamy jeden losowy fragment trasy.
// Tresc sugeruje start od pm = 0.1.
void mutateInversion(Individual& individual, double pm, Rng& rng);
