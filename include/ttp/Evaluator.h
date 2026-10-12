// Funkcja oceny G = zysk - czas, plus licznik wywolan.
#pragma once

#include "ttp/Distance.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"

class Evaluator {
public:
    Evaluator(const Problem& problem, const Distance& distance);

    // Dobiera przedmioty (packItems), liczy G i zapisuje wynik w osobniku.
    // Zwraca G.
    double evaluate(Individual& individual);

    // Ile razy wywolano evaluate
    long long getCount() const;
    void resetCount();

private:
    const Problem& problem_;
    const Distance& distance_;
    long long count_ = 0;
};
