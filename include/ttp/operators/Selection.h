// Selekcja: turniej, ruletka. Obie zwracaja INDEKS wybranego osobnika w populacji,
// zeby nie kopiowac osobnikow przy kazdym wyborze.
#pragma once

#include <vector>

#include "ttp/Individual.h"
#include "ttp/Rng.h"

// Turniej: losujemy tourSize osobnikow i wybieramy najlepszego z nich.
// Tour = 1 to wybor calkowicie losowy, Tour = pop_size to prawie zawsze najlepszy.
int tournamentSelection(const std::vector<Individual>& population, int tourSize, Rng& rng);

// Ruletka: szansa wyboru proporcjonalna do jakosci.
// G moze byc ujemne, wiec od kazdego G odejmujemy najgorsze G w populacji (skalowanie).
int rouletteSelection(const std::vector<Individual>& population, Rng& rng);
