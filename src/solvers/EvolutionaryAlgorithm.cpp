#include "ttp/solvers/EvolutionaryAlgorithm.h"

#include <algorithm>

#include "ttp/operators/Crossover.h"
#include "ttp/operators/Mutation.h"
#include "ttp/operators/Selection.h"
#include "ttp/solvers/Greedy.h"

static int selectParent(const std::vector<Individual>& population, const EAConfig& config, Rng& rng) {
    if (config.selection == SelectionType::Roulette) {
        return rouletteSelection(population, rng);
    }
    return tournamentSelection(population, config.tourSize, rng);
}

static Individual makeChild(const Individual& parent1, const Individual& parent2, const EAConfig& config, Rng& rng) {
    if (config.crossover == CrossoverType::PMX) {
        return crossoverPMX(parent1, parent2, rng);
    }
    return crossoverOX(parent1, parent2, rng);
}

static void mutate(Individual& individual, const EAConfig& config, Rng& rng) {
    if (config.mutation == MutationType::Swap) {
        mutateSwap(individual, config.pm, rng);
    } else {
        mutateInversion(individual, config.pm, rng);
    }
}

static GenerationStats computeGenerationStats(const std::vector<Individual>& population, int generation) {
    GenerationStats stats;
    stats.generation = generation;
    stats.best = population[0].fitness;
    stats.worst = population[0].fitness;
    double sum = 0.0;

    for (const Individual& individual : population) {
        if (individual.fitness > stats.best) {
            stats.best = individual.fitness;
        }
        if (individual.fitness < stats.worst) {
            stats.worst = individual.fitness;
        }
        sum += individual.fitness;
    }

    stats.avg = sum / population.size();
    return stats;
}

static bool isBetter(const Individual& a, const Individual& b) {
    return a.fitness > b.fitness;
}

EAResult runEvolutionaryAlgorithm(const Problem& problem, const Distance& distance, Evaluator& evaluator,
                                  const EAConfig& config, Rng& rng) {
    EAResult result;
    long long evaluationsAtStart = evaluator.getCount();

    // 1. Inicjalizacja: czesc populacji zachlanna (greedyInit), reszta losowa
    int greedyCount = static_cast<int>(config.greedyInit * config.popSize);

    std::vector<Individual> population;
    for (int i = 0; i < config.popSize; i++) {
        Individual individual;
        if (i < greedyCount) {
            int startCity = rng.uniformInt(0, problem.numCities - 1);
            individual = makeNearestNeighbourIndividual(problem, distance, startCity);
        } else {
            individual = makeRandomIndividual(problem, rng);
        }
        evaluator.evaluate(individual);
        population.push_back(individual);

        if (i == 0 || individual.fitness > result.best.fitness) {
            result.best = individual;
        }
    }
    result.history.push_back(computeGenerationStats(population, 0));

    // 2. Kolejne pokolenia
    for (int generation = 1; generation <= config.generations; generation++) {
        // Koniec po wyczerpaniu limitu ocen
        long long used = evaluator.getCount() - evaluationsAtStart;
        if (config.maxEvaluations > 0 && used >= config.maxEvaluations) {
            break;
        }

        std::vector<Individual> newPopulation;

        // Elitaryzm: najlepsi przechodza bez zmian
        if (config.elitism > 0) {
            std::vector<Individual> sorted = population;
            std::sort(sorted.begin(), sorted.end(), isBetter);
            for (int e = 0; e < config.elitism && e < config.popSize; e++) {
                newPopulation.push_back(sorted[e]);
            }
        }

        // Reszta populacji: selekcja, krzyzowanie, mutacja, ocena
        while (static_cast<int>(newPopulation.size()) < config.popSize) {
            int index1 = selectParent(population, config, rng);
            int index2 = selectParent(population, config, rng);

            Individual child;
            if (rng.uniformReal() < config.px) {
                child = makeChild(population[index1], population[index2], config, rng);
            } else {
                child = population[index1]; // bez krzyzowania: kopia rodzica (z jego ocena)
            }

            mutate(child, config, rng);

            // Oceniamy tylko zmienione osobniki
            if (!child.evaluated) {
                evaluator.evaluate(child);
            }

            if (child.fitness > result.best.fitness) {
                result.best = child;
            }

            newPopulation.push_back(child);
        }

        population = newPopulation;
        result.history.push_back(computeGenerationStats(population, generation));
    }

    return result;
}
