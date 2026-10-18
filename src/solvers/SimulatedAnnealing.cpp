// Implementacja SimulatedAnnealing.h
#include "ttp/solvers/SimulatedAnnealing.h"

#include <cmath>

#include "ttp/operators/Mutation.h"
#include "ttp/solvers/Greedy.h"

// Robi z osobnika jego sasiada: jedna zamiana dwoch miast albo jedno odwrocenie fragmentu
static void makeNeighbour(Individual& individual, MutationType move, Rng& rng) {
    int n = static_cast<int>(individual.route.size());
    int i = rng.uniformInt(0, n - 1);
    int j = rng.uniformInt(0, n - 1);

    if (move == MutationType::Swap) {
        swapCities(individual.route, i, j);
    } else {
        if (i > j) {
            int temp = i;
            i = j;
            j = temp;
        }
        reverseSegment(individual.route, i, j);
    }

    individual.evaluated = false;
}

// Automatyczna temperatura poczatkowa.
// Sprawdzamy 100 losowych ruchow i liczymy, o ile srednio pogarszaja wynik.
// Temperature dobieramy tak, zeby na starcie taki typowy gorszy ruch byl akceptowany w ok. 50% przypadkow:
// exp(-srednie_pogorszenie / T) = 0.5  ->  T = srednie_pogorszenie / ln(2)
static double estimateStartTemperature(const Problem& problem, Evaluator& evaluator, MutationType move, Rng& rng) {
    Individual start = makeRandomIndividual(problem, rng);
    evaluator.evaluate(start);

    double sumWorse = 0.0;
    int countWorse = 0;
    for (int k = 0; k < 100; k++) {
        Individual neighbour = start;
        makeNeighbour(neighbour, move, rng);
        evaluator.evaluate(neighbour);

        double delta = neighbour.fitness - start.fitness;
        if (delta < 0) {
            sumWorse += -delta;
            countWorse++;
        }
    }

    if (countWorse == 0) {
        return 1.0;
    }
    double averageWorse = sumWorse / countWorse;
    return averageWorse / std::log(2.0);
}

SAResult runSimulatedAnnealing(const Problem& problem, const Distance& distance, Evaluator& evaluator,
                               const SAConfig& config, Rng& rng) {
    SAResult result;

    // 1. Temperatura poczatkowa: podana albo dobrana automatycznie
    double temperature = config.startTemperature;
    if (temperature <= 0.0) {
        temperature = estimateStartTemperature(problem, evaluator, config.move, rng);
    }
    result.startTemperature = temperature;

    // 2. Rozwiazanie startowe: losowe albo zachlanne (najblizszy sasiad z losowego miasta)
    Individual current;
    if (config.greedyStart) {
        int startCity = rng.uniformInt(0, problem.numCities - 1);
        current = makeNearestNeighbourIndividual(problem, distance, startCity);
    } else {
        current = makeRandomIndividual(problem, rng);
    }
    evaluator.evaluate(current);
    result.best = current;
    result.history.push_back({0, result.best.fitness, current.fitness, temperature});

    // 3. Glowna petla
    for (int iteration = 1; iteration <= config.iterations; iteration++) {
        // 3a. Sasiad = kopia aktualnego rozwiazania z jednym ruchem
        Individual neighbour = current;
        makeNeighbour(neighbour, config.move, rng);
        evaluator.evaluate(neighbour);

        // 3b. Czy przechodzimy do sasiada?
        double delta = neighbour.fitness - current.fitness;  // > 0 oznacza poprawe
        if (delta >= 0) {
            current = neighbour;  // lepszy albo rowny: zawsze bierzemy
        } else {
            // gorszy: bierzemy z prawdopodobienstwem exp(delta / T)
            // (delta jest ujemna, wiec wynik jest miedzy 0 a 1)
            double acceptProbability = std::exp(delta / temperature);
            if (rng.uniformReal() < acceptProbability) {
                current = neighbour;
            }
        }

        // 3c. Zapamietujemy najlepsze rozwiazanie z calego przebiegu
        if (current.fitness > result.best.fitness) {
            result.best = current;
        }

        // 3d. Schladzanie
        temperature = temperature * config.coolingRate;

        // 3e. Co logEvery iteracji zapisujemy stan do historii
        if (iteration % config.logEvery == 0) {
            result.history.push_back({iteration, result.best.fitness, current.fitness, temperature});
        }
    }

    return result;
}
