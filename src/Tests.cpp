#include "ttp/Tests.h"

#include <iostream>
#include <string>
#include <vector>

#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Loader.h"
#include "ttp/Rng.h"
#include "ttp/operators/Crossover.h"
#include "ttp/operators/Mutation.h"

bool testTinyInstance() {
    const Problem problem = loadProblem(std::string(TTP_TESTS_DIR) + "/tiny.ttp");
    const Distance distance(problem);
    Evaluator evaluator(problem, distance);

    Individual individual = makeIdentityIndividual(problem);
    double G = evaluator.evaluate(individual);

    double expected = -49.25;
    double diff = G - expected;
    bool ok = diff > -0.0001 && diff < 0.0001;

    std::cout << "Test tiny.ttp: G = " << G << " (oczekiwane " << expected << ") -> "
              << (ok ? "OK" : "BLAD!") << "\n\n";
    return ok;
}

void printRoute(const std::string& label, const std::vector<int>& route) {
    std::cout << label << " |";
    for (int city : route) {
        std::cout << city;
    }
    std::cout << "|\n";
}

bool testOperators() {
    std::cout << "=== Test operatorow ===\n";

    std::vector<int> route = {5, 6, 7, 1, 2, 3, 4};
    std::vector<int> swapped = route;
    swapCities(swapped, 1, 5);
    printRoute("Swap      5671234 ->", swapped);
    std::cout << "                     (oczekiwane |5371264|)\n";

    std::vector<int> inverted = route;
    reverseSegment(inverted, 1, 5);
    printRoute("Inwersja  5671234 ->", inverted);
    std::cout << "                     (oczekiwane |5321764|)\n";

    std::vector<int> parent1 = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> parent2 = {8, 2, 6, 7, 1, 5, 4, 0, 3};
    printRoute("Rodzic 1           ", parent1);
    printRoute("Rodzic 2           ", parent2);
    printRoute("OX  [3,5]          ", orderCrossover(parent1, parent2, 3, 5));
    std::cout << "                     (oczekiwane |826345710|)\n";
    printRoute("PMX [3,5]          ", pmxCrossover(parent1, parent2, 3, 5));
    std::cout << "                     (oczekiwane |826345107|)\n";

    const Problem problem = loadProblem(std::string(TTP_DATA_DIR) + "/hard/a280_n1395_uncorr-similar-weights_05.ttp");
    Rng rng(123);
    int errors = 0;
    for (int t = 0; t < 10000; t++) {
        Individual p1 = makeRandomIndividual(problem, rng);
        Individual p2 = makeRandomIndividual(problem, rng);

        Individual childOX = crossoverOX(p1, p2, rng);
        Individual childPMX = crossoverPMX(p1, p2, rng);
        mutateSwap(childOX, 0.01, rng);
        mutateInversion(childPMX, 1.0, rng);

        if (!isValidRoute(childOX, problem.numCities)) {
            errors++;
        }
        if (!isValidRoute(childPMX, problem.numCities)) {
            errors++;
        }
    }
    std::cout << "10 000 losowych testow: bledow = " << errors << " (oczekiwane 0)\n\n";
    return errors == 0;
}
