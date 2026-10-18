// Zachlanny plan przedmiotow dla zadanej trasy (TSP -> TTP).
#pragma once

#include "ttp/Distance.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"

// Wypelnia individual.picked na podstawie trasy individual.route.
// Przedmioty oceniamy wzorem: profit / (weight * droga_do_konca_trasy)
// i bierzemy od najlepszego, dopoki mieszcza sie w plecaku.
// W miescie startowym nic nie zbieramy.
//
// profitCheck = true (wersja "smart"): przedmiot bierzemy tylko wtedy, gdy jego zysk jest wiekszy
// niz szacowany dodatkowy czas jazdy przez niego (spowolnienie na drodze do konca trasy).
void packItems(const Problem& problem, const Distance& distance, Individual& individual, bool profitCheck = false);
