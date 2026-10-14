// Implementacja Selection.h
#include "ttp/operators/Selection.h"

int tournamentSelection(const std::vector<Individual>& population, int tourSize, Rng& rng) {
    int size = static_cast<int>(population.size());

    // Pierwszy uczestnik turnieju
    int bestIndex = rng.uniformInt(0, size - 1);

    // Pozostali uczestnicy: jesli ktorys jest lepszy, zostaje zwyciezca
    for (int t = 1; t < tourSize; t++) {
        int index = rng.uniformInt(0, size - 1);
        if (population[index].fitness > population[bestIndex].fitness) {
            bestIndex = index;
        }
    }

    return bestIndex;
}

int rouletteSelection(const std::vector<Individual>& population, Rng& rng) {
    int size = static_cast<int>(population.size());

    // 1. Najgorsze G w populacji
    double worst = population[0].fitness;
    for (int i = 1; i < size; i++) {
        if (population[i].fitness < worst) {
            worst = population[i].fitness;
        }
    }

    // 2. Waga osobnika = jego G - najgorsze G (zawsze >= 0)
    double total = 0.0;
    for (int i = 0; i < size; i++) {
        total += population[i].fitness - worst;
    }

    // Wszyscy rowni: wybieramy losowo
    if (total <= 0.0) {
        return rng.uniformInt(0, size - 1);
    }

    // 3. Krecimy kolem: losujemy punkt na kole i szukamy, na czyje pole wypadl
    double point = rng.uniformReal() * total;
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += population[i].fitness - worst;
        if (sum >= point) {
            return i;
        }
    }

    return size - 1; // na wypadek bledow zaokraglen
}
