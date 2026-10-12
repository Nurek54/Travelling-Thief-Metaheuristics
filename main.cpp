#include <chrono>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Loader.h"
#include "ttp/Rng.h"
#include "ttp/Stats.h"
#include "ttp/solvers/Greedy.h"
#include "ttp/solvers/RandomSearch.h"

// Test funkcji oceny na malym przykladzie policzonym recznie (tests/tiny_expected.md)
bool testTinyInstance() {
    const Problem problem = loadProblem(std::string(TTP_TESTS_DIR) + "/tiny.ttp");
    const Distance distance(problem);
    Evaluator evaluator(problem, distance);

    Individual individual = makeIdentityIndividual(problem); // trasa 0 1 2 3
    double G = evaluator.evaluate(individual);

    double expected = -49.25;
    double diff = G - expected;
    bool ok = diff > -0.0001 && diff < 0.0001;

    std::cout << "Test tiny.ttp: G = " << G << " (oczekiwane " << expected << ") -> "
              << (ok ? "OK" : "BLAD!") << "\n\n";
    return ok;
}

// Wypisuje jeden wiersz wynikow na ekran i do pliku CSV
void printResult(std::ofstream& csv, const std::string& instance, const std::string& method,
                 const SolverResult& result, long long evaluations, double timeMs) {
    Stats stats = computeStats(result.values);

    std::cout << "  " << method
              << "  best = " << stats.best
              << "  worst = " << stats.worst
              << "  avg = " << stats.avg
              << "  std = " << stats.std
              << "  ocen = " << evaluations
              << "  czas = " << timeMs << " ms\n";

    csv << instance << ';' << method << ';'
        << stats.best << ';' << stats.worst << ';' << stats.avg << ';' << stats.std << ';'
        << evaluations << ';' << timeMs << '\n';
}

int main() {
    // Liczby zawsze z 2 miejscami po przecinku (bez zapisu 1.29e+06)
    std::cout << std::fixed << std::setprecision(2);

    try {
        if (!testTinyInstance()) {
            std::cerr << "Funkcja oceny liczy zle, przerywam.\n";
            return 1;
        }

        // 7 instancji: 2 latwe, 2 srednie, 2 trudne, 1 extra
        std::vector<std::string> instances = {
            "easy/a280_n279_uncorr_01.ttp",
            "easy/a280_n279_bounded-strongly-corr_01.ttp",
            "medium/a280_n1395_uncorr-similar-weights_05.ttp",
            "medium/a280_n1395_bounded-strongly-corr_05.ttp",
            "hard/a280_n2790_uncorr_10.ttp",
            "hard/a280_n2790_bounded-strongly-corr_10.ttp",
            "extra/a280_n837_uncorr-similar-weights_05.ttp",
        };

        int randomCount = 10000; // Alg. losowy [10k]
        unsigned int seed = 42;

        std::ofstream csv(std::string(TTP_RESULTS_DIR) + "/zajecia1.csv");
        if (!csv.is_open()) {
            std::cerr << "Nie mozna utworzyc pliku results/zajecia1.csv\n";
            return 1;
        }
        csv << std::fixed << std::setprecision(2);
        csv << "instance;method;best;worst;avg;std;evaluations;time_ms\n";

        for (const std::string& instance : instances) {
            const Problem problem = loadProblem(std::string(TTP_DATA_DIR) + "/" + instance);
            const Distance distance(problem);
            Evaluator evaluator(problem, distance);

            std::cout << instance << '\n';

            // Metoda losowa
            {
                Rng rng(seed);
                evaluator.resetCount();
                auto start = std::chrono::steady_clock::now();

                SolverResult result = runRandomSearch(problem, evaluator, rng, randomCount);

                auto end = std::chrono::steady_clock::now();
                double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
                printResult(csv, instance, "random", result, evaluator.getCount(), timeMs);
            }

            // Metoda zachlanna
            {
                evaluator.resetCount();
                auto start = std::chrono::steady_clock::now();

                SolverResult result = runGreedy(problem, distance, evaluator);

                auto end = std::chrono::steady_clock::now();
                double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
                printResult(csv, instance, "greedy", result, evaluator.getCount(), timeMs);
            }

            std::cout << '\n';
        }

        std::cout << "Wyniki zapisane w results/zajecia1.csv\n";
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
