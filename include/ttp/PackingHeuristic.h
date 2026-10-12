// Zachlanny plan przedmiotow dla zadanej trasy (TSP -> TTP).
#pragma once

#include "ttp/Distance.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"

// Wypelnia individual.picked na podstawie trasy individual.route.
// Przedmioty oceniamy wzorem: profit / (weight * droga_do_konca_trasy)
// i bierzemy od najlepszego, dopoki mieszcza sie w plecaku.
// W miescie startowym nic nie zbieramy.
void packItems(const Problem& problem, const Distance& distance, Individual& individual);
