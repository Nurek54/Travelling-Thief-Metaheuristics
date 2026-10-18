// Funkcja oceny G = zysk - czas, plus licznik wywolan.
#pragma once

#include "ttp/Distance.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"

class Evaluator {
public:
    // smartPacking = true: pakowanie z kontrola oplacalnosci (patrz PackingHeuristic.h)
    Evaluator(const Problem& problem, const Distance& distance, bool smartPacking = false);

    // Dobiera przedmioty (packItems), liczy G i zapisuje wynik w osobniku.
    // Zwraca G.
    double evaluate(Individual& individual);

    // Ile razy wywolano evaluate
    long long getCount() const;
    void resetCount();

private:
    const Problem& problem_;
    const Distance& distance_;
    bool smartPacking_ = false;
    long long count_ = 0;
};
