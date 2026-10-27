#include "ttp/Experiments.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "ttp/Loader.h"
#include "ttp/Logger.h"
#include "ttp/Rng.h"
#include "ttp/Stats.h"
#include "ttp/solvers/EvolutionaryAlgorithm.h"
#include "ttp/solvers/Greedy.h"
#include "ttp/solvers/RandomSearch.h"
#include "ttp/solvers/SimulatedAnnealing.h"

void printResult(std::ofstream& csv, const std::string& instance, const std::string& method,
                 const SolverResult& result, long long evaluations, double timeMs) {
    Stats stats = computeStats(result.values);

    std::cout << "  " << std::left << std::setw(9) << method << std::right
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

std::string historyPath(const std::string& folder, const std::string& instance, int run) {
    std::string name = std::filesystem::path(instance).stem().string();
    std::string runNumber = std::to_string(run + 1);
    if (run + 1 < 10) {
        runNumber = "0" + runNumber;
    }
    return std::string(TTP_RESULTS_DIR) + "/" + folder + "/" + name + "_run" + runNumber + ".csv";
}

void runEaSeries(std::ofstream& csv, const std::string& instance, const std::string& methodName,
                 const Problem& problem, const Distance& distance, Evaluator& evaluator,
                 const EAConfig& config, int runs, unsigned int seed, const std::string& historyFolder) {
    SolverResult result;
    long long totalEvaluations = 0;
    auto start = std::chrono::steady_clock::now();

    for (int run = 0; run < runs; run++) {
        Rng rng(seed + run);
        evaluator.resetCount();

        EAResult ea = runEvolutionaryAlgorithm(problem, distance, evaluator, config, rng);
        totalEvaluations += evaluator.getCount();
        result.values.push_back(ea.best.fitness);

        if (run == 0 || ea.best.fitness > result.best.fitness) {
            result.best = ea.best;
        }
        if (historyFolder != "") {
            saveHistoryCsv(historyPath(historyFolder, instance, run), ea.history);
        }
    }

    auto end = std::chrono::steady_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    printResult(csv, instance, methodName, result, totalEvaluations / runs, timeMs / runs);
}

void runSaSeries(std::ofstream& csv, const std::string& instance, const std::string& methodName,
                 const Problem& problem, const Distance& distance, Evaluator& evaluator,
                 const SAConfig& config, int runs, unsigned int seed, const std::string& historyFolder) {
    SolverResult result;
    long long totalEvaluations = 0;
    auto start = std::chrono::steady_clock::now();

    for (int run = 0; run < runs; run++) {
        Rng rng(seed + run);
        evaluator.resetCount();

        SAResult sa = runSimulatedAnnealing(problem, distance, evaluator, config, rng);
        totalEvaluations += evaluator.getCount();
        result.values.push_back(sa.best.fitness);

        if (run == 0 || sa.best.fitness > result.best.fitness) {
            result.best = sa.best;
        }
        if (historyFolder != "") {
            saveSAHistoryCsv(historyPath(historyFolder, instance, run), sa.history);
        }
    }

    auto end = std::chrono::steady_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    printResult(csv, instance, methodName, result, totalEvaluations / runs, timeMs / runs);
}

int runFullComparison() {
    std::vector<std::string> instances = {
        "easy/berlin52_n255_uncorr-similar-weights_05.ttp",
        "easy/berlin52_n255_bounded-strongly-corr_05.ttp",
        "medium/kroA150_n745_uncorr-similar-weights_05.ttp",
        "medium/kroA150_n745_bounded-strongly-corr_05.ttp",
        "hard/a280_n1395_uncorr-similar-weights_05.ttp",
        "hard/a280_n1395_bounded-strongly-corr_05.ttp",
        "extra/kroA100_n495_uncorr-similar-weights_05.ttp",
    };

    unsigned int seed = 42;
    int runs = 10;
    int budget = 10000;

    EAConfig eaStart;
    SAConfig saStart;

    EAConfig eaTuned;
    eaTuned.generations = 1000;
    eaTuned.maxEvaluations = budget;
    eaTuned.greedyInit = 1.0;
    eaTuned.mutation = MutationType::Inversion;
    eaTuned.pm = 0.5;
    eaTuned.px = 1.0;
    eaTuned.elitism = 5;
    eaTuned.tourSize = 10;

    SAConfig saTuned;
    saTuned.iterations = budget - 1;
    saTuned.startTemperature = 10.0;
    saTuned.move = MutationType::Inversion;
    saTuned.greedyStart = true;

    std::cout << "Budzet: " << budget << " ocen na uruchomienie, " << runs << " uruchomien\n"
              << "ea_start: " << describeConfig(eaStart) << '\n'
              << "sa_start: " << describeConfig(saStart) << '\n'
              << "ea:       " << describeConfig(eaTuned) << '\n'
              << "sa:       " << describeConfig(saTuned) << "\n\n";

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

        {
            Rng rng(seed);
            evaluator.resetCount();
            auto start = std::chrono::steady_clock::now();

            SolverResult result = runRandomSearch(problem, evaluator, rng, budget);

            auto end = std::chrono::steady_clock::now();
            double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
            printResult(csv, instance, "random", result, evaluator.getCount(), timeMs);
        }

        {
            evaluator.resetCount();
            auto start = std::chrono::steady_clock::now();

            SolverResult result = runGreedy(problem, distance, evaluator);

            auto end = std::chrono::steady_clock::now();
            double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
            printResult(csv, instance, "greedy", result, evaluator.getCount(), timeMs);
        }

        runEaSeries(csv, instance, "ea_start", problem, distance, evaluator, eaStart, runs, seed, "");
        runSaSeries(csv, instance, "sa_start", problem, distance, evaluator, saStart, runs, seed, "");
        runEaSeries(csv, instance, "ea", problem, distance, evaluator, eaTuned, runs, seed, "history");
        runSaSeries(csv, instance, "sa", problem, distance, evaluator, saTuned, runs, seed, "history_sa");

        std::cout << '\n';
    }

    std::cout << "Wyniki zapisane w results/wyniki.csv\n"
              << "Historia pokolen EA zapisana w results/history/\n"
              << "Historia SA zapisana w results/history_sa/\n"
              << "Wykresy: python scripts/plot_generations.py\n"
              << "Tabela:  python scripts/make_table.py\n";
    return 0;
}
