#include <chrono>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "ttp/Config.h"
#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Individual.h"
#include "ttp/Loader.h"
#include "ttp/Logger.h"
#include "ttp/Rng.h"
#include "ttp/Stats.h"
#include "ttp/operators/Crossover.h"
#include "ttp/operators/Mutation.h"
#include "ttp/solvers/EvolutionaryAlgorithm.h"
#include "ttp/solvers/Greedy.h"
#include "ttp/solvers/RandomSearch.h"
#include "ttp/solvers/SimulatedAnnealing.h"

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

// Wypisuje trase jako ciag cyfr, np. |5671234|
void printRoute(const std::string& label, const std::vector<int>& route) {
    std::cout << label << " |";
    for (int city : route) {
        std::cout << city;
    }
    std::cout << "|\n";
}

// Test operatorow: przyklady z tresci + sprawdzenie, ze dzieci to zawsze poprawne permutacje
bool testOperators() {
    std::cout << "=== Test operatorow ===\n";

    // Przyklady z Rysunku 2 w tresci
    std::vector<int> route = {5, 6, 7, 1, 2, 3, 4};
    std::vector<int> swapped = route;
    swapCities(swapped, 1, 5);
    printRoute("Swap      5671234 ->", swapped);
    std::cout << "                     (oczekiwane |5371264|)\n";

    std::vector<int> inverted = route;
    reverseSegment(inverted, 1, 5);
    printRoute("Inwersja  5671234 ->", inverted);
    std::cout << "                     (oczekiwane |5321764|)\n";

    // Krzyzowanie na malym przykladzie, fragment [3, 5]
    std::vector<int> parent1 = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> parent2 = {8, 2, 6, 7, 1, 5, 4, 0, 3};
    printRoute("Rodzic 1           ", parent1);
    printRoute("Rodzic 2           ", parent2);
    printRoute("OX  [3,5]          ", orderCrossover(parent1, parent2, 3, 5));
    std::cout << "                     (oczekiwane |826345710|)\n";
    printRoute("PMX [3,5]          ", pmxCrossover(parent1, parent2, 3, 5));
    std::cout << "                     (oczekiwane |826345107|)\n";

    // 10 000 losowych krzyzowan i mutacji na 280 miastach: dziecko zawsze musi byc permutacja
    const Problem problem = loadProblem(std::string(TTP_DATA_DIR) + "/easy/a280_n279_uncorr_01.ttp");
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
        if (!testOperators()) {
            std::cerr << "Operatory psuja trasy, przerywam.\n";
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

        // Algorytm ewolucyjny: parametry startowe z tresci
        EAConfig eaConfig;
        int eaRuns = 10; // tresc: min. 10 uruchomien, srednia i odchylenie
        std::cout << "EA: " << describeConfig(eaConfig) << ", " << eaRuns << " uruchomien\n";

        // Symulowane wyzarzanie: tyle samo uruchomien
        SAConfig saConfig;
        int saRuns = 10;
        std::cout << "SA: " << describeConfig(saConfig) << ", " << saRuns << " uruchomien\n\n";

        std::ofstream csv(std::string(TTP_RESULTS_DIR) + "/wyniki.csv");
        if (!csv.is_open()) {
            std::cerr << "Nie mozna utworzyc pliku results/wyniki.csv\n";
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

            // Algorytm ewolucyjny: 10 uruchomien z roznymi seedami
            {
                SolverResult result; // values = najlepsze G z kazdego uruchomienia
                long long totalEvaluations = 0;
                auto start = std::chrono::steady_clock::now();

                for (int run = 0; run < eaRuns; run++) {
                    Rng rng(seed + run);
                    evaluator.resetCount();

                    EAResult ea = runEvolutionaryAlgorithm(problem, evaluator, eaConfig, rng);
                    totalEvaluations += evaluator.getCount();

                    // Historia pokolen do CSV, np. results/history/a280_n279_uncorr_01_run01.csv
                    std::string name = std::filesystem::path(instance).stem().string();
                    std::string runNumber = std::to_string(run + 1);
                    if (run + 1 < 10) {
                        runNumber = "0" + runNumber;
                    }
                    saveHistoryCsv(std::string(TTP_RESULTS_DIR) + "/history/" + name + "_run" + runNumber + ".csv", ea.history);
                    result.values.push_back(ea.best.fitness);

                    if (run == 0 || ea.best.fitness > result.best.fitness) {
                        result.best = ea.best;
                    }

                    // Dla pierwszego uruchomienia pokazujemy, jak rosnie najlepszy wynik
                    if (run == 0) {
                        std::cout << "  ea (run 1) best w pokoleniach:";
                        for (const GenerationStats& stats : ea.history) {
                            if (stats.generation % 25 == 0) {
                                std::cout << "  [" << stats.generation << "] " << stats.best;
                            }
                        }
                        std::cout << '\n';
                    }
                }

                auto end = std::chrono::steady_clock::now();
                double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
                // srednia liczba ocen i sredni czas na jedno uruchomienie
                printResult(csv, instance, "ea", result, totalEvaluations / eaRuns, timeMs / eaRuns);
            }

            // Symulowane wyzarzanie: 10 uruchomien z roznymi seedami
            {
                SolverResult result; // values = najlepsze G z kazdego uruchomienia
                long long totalEvaluations = 0;
                auto start = std::chrono::steady_clock::now();

                for (int run = 0; run < saRuns; run++) {
                    Rng rng(seed + run);
                    evaluator.resetCount();

                    SAResult sa = runSimulatedAnnealing(problem, evaluator, saConfig, rng);
                    totalEvaluations += evaluator.getCount();
                    result.values.push_back(sa.best.fitness);

                    if (run == 0 || sa.best.fitness > result.best.fitness) {
                        result.best = sa.best;
                    }

                    // Historia do CSV, np. results/history_sa/a280_n279_uncorr_01_run01.csv
                    std::string name = std::filesystem::path(instance).stem().string();
                    std::string runNumber = std::to_string(run + 1);
                    if (run + 1 < 10) {
                        runNumber = "0" + runNumber;
                    }
                    saveSAHistoryCsv(std::string(TTP_RESULTS_DIR) + "/history_sa/" + name + "_run" + runNumber + ".csv", sa.history);

                    if (run == 0) {
                        std::cout << "  sa (run 1) T0 = " << sa.startTemperature << ", best w iteracjach:";
                        for (const SAStats& stats : sa.history) {
                            if (stats.iteration % 2500 == 0) {
                                std::cout << "  [" << stats.iteration << "] " << stats.best;
                            }
                        }
                        std::cout << '\n';
                    }
                }

                auto end = std::chrono::steady_clock::now();
                double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
                printResult(csv, instance, "sa", result, totalEvaluations / saRuns, timeMs / saRuns);
            }

            std::cout << '\n';
        }

        std::cout << "Wyniki zapisane w results/wyniki.csv\n"
                  << "Historia pokolen EA zapisana w results/history/\n"
                  << "Historia SA zapisana w results/history_sa/\n"
                  << "Wykresy: python scripts/plot_generations.py\n"
                  << "Tabela:  python scripts/make_table.py\n";
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
