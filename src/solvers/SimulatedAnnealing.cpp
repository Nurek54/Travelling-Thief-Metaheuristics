#include "ttp/solvers/SimulatedAnnealing.h"

#include <cmath>

#include "ttp/operators/Mutation.h"
#include "ttp/solvers/Greedy.h"

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

// Automatyczne T0: srednie pogorszenie po 100 losowych ruchach od startu.
// Taki ruch ma byc na starcie przyjmowany z prawdopodobienstwem acceptance:
// T = srednie_pogorszenie / (-ln(acceptance))
static double estimateStartTemperature(const Individual& start, Evaluator& evaluator, MutationType move,
                                       double acceptance, Rng& rng) {
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
    return averageWorse / -std::log(acceptance);
}

SAResult runSimulatedAnnealing(const Problem& problem, const Distance& distance, Evaluator& evaluator,
                               const SAConfig& config, Rng& rng) {
    SAResult result;

    // 1. Rozwiazanie startowe: losowe albo zachlanne (najblizszy sasiad z losowego miasta)
    Individual current;
    if (config.greedyStart) {
        int startCity = rng.uniformInt(0, problem.numCities - 1);
        current = makeNearestNeighbourIndividual(problem, distance, startCity);
    } else {
        current = makeRandomIndividual(problem, rng);
    }
    evaluator.evaluate(current);

    // 2. Temperatura poczatkowa: podana albo automatyczna
    double temperature = config.startTemperature;
    if (temperature <= 0.0) {
        temperature = estimateStartTemperature(current, evaluator, config.move, config.startAcceptance, rng);
    }
    result.startTemperature = temperature;
    result.best = current;
    result.history.push_back({0, result.best.fitness, current.fitness, temperature});

    // 3. Glowna petla
    for (int iteration = 1; iteration <= config.iterations; iteration++) {
        // Sasiad = kopia aktualnego rozwiazania z jednym ruchem
        Individual neighbour = current;
        makeNeighbour(neighbour, config.move, rng);
        evaluator.evaluate(neighbour);

        // Czy przechodzimy do sasiada?
        double delta = neighbour.fitness - current.fitness;
        if (delta >= 0) {
            current = neighbour;
        } else {
            // gorszy: bierzemy z prawdopodobienstwem exp(delta / T)
            double acceptProbability = std::exp(delta / temperature);
            if (rng.uniformReal() < acceptProbability) {
                current = neighbour;
            }
        }

        if (current.fitness > result.best.fitness) {
            result.best = current;
        }

        // Schladzanie
        temperature = temperature * config.coolingRate;

        if (iteration % config.logEvery == 0) {
            result.history.push_back({iteration, result.best.fitness, current.fitness, temperature});
        }
    }

    return result;
}
