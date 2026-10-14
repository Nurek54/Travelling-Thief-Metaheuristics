// Implementacja EvolutionaryAlgorithm.h
#include "ttp/solvers/EvolutionaryAlgorithm.h"

#include <algorithm>

#include "ttp/operators/Crossover.h"
#include "ttp/operators/Mutation.h"
#include "ttp/operators/Selection.h"

// Wybiera rodzica wedlug ustawionej metody selekcji
static int selectParent(const std::vector<Individual>& population, const EAConfig& config, Rng& rng) {
    if (config.selection == SelectionType::Roulette) {
        return rouletteSelection(population, rng);
    }
    return tournamentSelection(population, config.tourSize, rng);
}

// Tworzy dziecko wedlug ustawionego krzyzowania
static Individual makeChild(const Individual& parent1, const Individual& parent2, const EAConfig& config, Rng& rng) {
    if (config.crossover == CrossoverType::PMX) {
        return crossoverPMX(parent1, parent2, rng);
    }
    return crossoverOX(parent1, parent2, rng);
}

// Mutuje osobnika wedlug ustawionej mutacji
static void mutate(Individual& individual, const EAConfig& config, Rng& rng) {
    if (config.mutation == MutationType::Swap) {
        mutateSwap(individual, config.pm, rng);
    } else {
        mutateInversion(individual, config.pm, rng);
    }
}

// best / avg / worst w populacji
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

// Do sortowania: lepszy (wieksze G) idzie na poczatek
static bool isBetter(const Individual& a, const Individual& b) {
    return a.fitness > b.fitness;
}

EAResult runEvolutionaryAlgorithm(const Problem& problem, Evaluator& evaluator, const EAConfig& config, Rng& rng) {
    EAResult result;

    // 1. Inicjalizacja: losowa populacja i jej ocena
    std::vector<Individual> population;
    for (int i = 0; i < config.popSize; i++) {
        Individual individual = makeRandomIndividual(problem, rng);
        evaluator.evaluate(individual);
        population.push_back(individual);

        if (i == 0 || individual.fitness > result.best.fitness) {
            result.best = individual;
        }
    }
    result.history.push_back(computeGenerationStats(population, 0));

    // 2. Kolejne pokolenia
    for (int generation = 1; generation <= config.generations; generation++) {
        std::vector<Individual> newPopulation;

        // 2a. Elitaryzm: najlepsi przechodza bez zmian
        if (config.elitism > 0) {
            std::vector<Individual> sorted = population;
            std::sort(sorted.begin(), sorted.end(), isBetter);
            for (int e = 0; e < config.elitism && e < config.popSize; e++) {
                newPopulation.push_back(sorted[e]);
            }
        }

        // 2b. Reszta populacji: selekcja -> krzyzowanie -> mutacja -> ocena
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

            // Oceniamy tylko, jesli osobnik sie zmienil (oszczedzamy wywolania oceny)
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
