#include <exception>
#include <iostream>
#include <string>

#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Loader.h"
#include "ttp/Rng.h"

int main() {
    try {
        // TEST 1: maly przyklad policzony recznie (tests/tiny_expected.md)
        {
            const Problem problem = loadProblem(std::string(TTP_TESTS_DIR) + "/tiny.ttp");
            const Distance distance(problem);
            Evaluator evaluator(problem, distance);

            Individual individual = makeIdentityIndividual(problem); // trasa 0 1 2 3
            const double G = evaluator.evaluate(individual);

            std::cout << "=== TEST tiny.ttp ===\n"
                      << "Wybrane przedmioty: ";
            for (int k = 0; k < problem.numItems; k++) {
                std::cout << static_cast<int>(individual.picked[k]) << ' ';
            }
            std::cout << "(oczekiwane 1 1 0)\n"
                      << "Zysk: " << individual.totalProfit << " (oczekiwane 30)\n"
                      << "Czas: " << individual.travelTime << " (oczekiwane 79.25)\n"
                      << "G:    " << G << " (oczekiwane -49.25)\n\n";
        }

        // TEST 2: losowe osobniki na prawdziwej instancji
        {
            const std::string path = std::string(TTP_DATA_DIR) + "/easy/a280_n279_uncorr_01.ttp";
            const Problem problem = loadProblem(path);
            const Distance distance(problem);
            Evaluator evaluator(problem, distance);
            Rng rng(42);

            std::cout << "=== 5 losowych osobnikow: " << problem.name << " ===\n";
            for (int i = 0; i < 5; i++) {
                Individual individual = makeRandomIndividual(problem, rng);
                const double G = evaluator.evaluate(individual);
                std::cout << "G = " << G
                          << "  zysk = " << individual.totalProfit
                          << "  waga = " << individual.totalWeight << "/" << problem.capacity
                          << "  czas = " << individual.travelTime << '\n';
            }
            std::cout << "Liczba ocen: " << evaluator.getCount() << '\n';
        }
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
