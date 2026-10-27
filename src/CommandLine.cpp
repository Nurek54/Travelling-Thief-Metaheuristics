#include "ttp/CommandLine.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "ttp/Config.h"
#include "ttp/Distance.h"
#include "ttp/Evaluator.h"
#include "ttp/Loader.h"
#include "ttp/Rng.h"
#include "ttp/Stats.h"
#include "ttp/solvers/EvolutionaryAlgorithm.h"
#include "ttp/solvers/Greedy.h"
#include "ttp/solvers/RandomSearch.h"
#include "ttp/solvers/SimulatedAnnealing.h"

void printHelp() {
    std::cout <<
        "Uzycie: travelling_thief_metaheuristics [--klucz wartosc] ...\n"
        "Bez argumentow: pelne porownanie metod na 7 instancjach.\n\n"
        "Wspolne:\n"
        "  --method    random | greedy | ea | sa       (domyslnie ea)\n"
        "  --instance  sciezka wzgledem data/          (domyslnie extra/kroA100_n495_uncorr-similar-weights_05.ttp)\n"
        "  --runs      liczba uruchomien ea/sa         (domyslnie 10)\n"
        "  --seed      seed pierwszego uruchomienia    (domyslnie 42)\n"
        "  --out       plik CSV, do ktorego DOPISAC wynik (wzgledem results/, domyslnie experiments.csv)\n"
        "  --label     dowolny opis wiersza, np. pm=0.2\n"
        "  --packing   simple | smart (smart: bierz przedmiot tylko, gdy sie oplaca)\n"
        "Losowy:\n"
        "  --count     liczba losowych osobnikow       (domyslnie 10000)\n"
        "EA:\n"
        "  --pop --gen --px --pm --tour --elite --max-evals (limit ocen, 0 = brak)\n"
        "  --sel tournament|roulette   --cross ox|pmx   --mut swap|inversion\n"
        "  --greedy-init 0.0-1.0       (czesc populacji startowej z algorytmu zachlannego)\n"
        "SA:\n"
        "  --iter --t0 (0 = auto) --accept (dla auto T0, np. 0.5) --cooling --move swap|inversion --greedy-start 0|1\n";
}

std::string getText(const std::map<std::string, std::string>& args, const std::string& key, const std::string& defaultValue) {
    if (args.count(key) == 0) {
        return defaultValue;
    }
    return args.at(key);
}

int getInt(const std::map<std::string, std::string>& args, const std::string& key, int defaultValue) {
    if (args.count(key) == 0) {
        return defaultValue;
    }
    return std::stoi(args.at(key));
}

double getDouble(const std::map<std::string, std::string>& args, const std::string& key, double defaultValue) {
    if (args.count(key) == 0) {
        return defaultValue;
    }
    return std::stod(args.at(key));
}

SelectionType parseSelection(const std::string& text) {
    if (text == "tournament") return SelectionType::Tournament;
    if (text == "roulette") return SelectionType::Roulette;
    throw std::runtime_error("Nieznana selekcja: " + text + " (dozwolone: tournament, roulette)");
}

CrossoverType parseCrossover(const std::string& text) {
    if (text == "ox") return CrossoverType::OX;
    if (text == "pmx") return CrossoverType::PMX;
    throw std::runtime_error("Nieznane krzyzowanie: " + text + " (dozwolone: ox, pmx)");
}

MutationType parseMutation(const std::string& text) {
    if (text == "swap") return MutationType::Swap;
    if (text == "inversion") return MutationType::Inversion;
    throw std::runtime_error("Nieznana mutacja/ruch: " + text + " (dozwolone: swap, inversion)");
}

int runSingleExperiment(int argc, char* argv[]) {
    std::map<std::string, std::string> args;
    for (int i = 1; i < argc; i++) {
        std::string key = argv[i];
        if (key == "--help" || key == "-h") {
            printHelp();
            return 0;
        }
        if (key.rfind("--", 0) != 0) {
            throw std::runtime_error("Oczekiwano argumentu --klucz, a jest: " + key);
        }
        if (i + 1 >= argc) {
            throw std::runtime_error("Brak wartosci dla " + key);
        }
        args[key.substr(2)] = argv[i + 1];
        i++;
    }

    std::vector<std::string> known = {
        "method", "instance", "runs", "seed", "out", "label", "count",
        "pop", "gen", "px", "pm", "tour", "elite", "sel", "cross", "mut", "greedy-init", "max-evals", "packing",
        "iter", "t0", "accept", "cooling", "move", "greedy-start"
    };
    for (const auto& pair : args) {
        bool found = false;
        for (const std::string& name : known) {
            if (pair.first == name) {
                found = true;
            }
        }
        if (!found) {
            throw std::runtime_error("Nieznany argument: --" + pair.first + " (zobacz --help)");
        }
    }

    std::string method = getText(args, "method", "ea");
    std::string instance = getText(args, "instance", "extra/kroA100_n495_uncorr-similar-weights_05.ttp");
    int runs = getInt(args, "runs", 10);
    unsigned int seed = static_cast<unsigned int>(getInt(args, "seed", 42));
    std::string label = getText(args, "label", "");

    std::filesystem::path outPath = getText(args, "out", "experiments.csv");
    if (outPath.is_relative()) {
        outPath = std::filesystem::path(TTP_RESULTS_DIR) / outPath;
    }

    std::string packing = getText(args, "packing", "simple");
    if (packing != "simple" && packing != "smart") {
        throw std::runtime_error("Nieznane pakowanie: " + packing + " (dozwolone: simple, smart)");
    }

    const Problem problem = loadProblem(std::string(TTP_DATA_DIR) + "/" + instance);
    const Distance distance(problem);
    Evaluator evaluator(problem, distance, packing == "smart");

    SolverResult result;
    long long totalEvaluations = 0;
    std::string configText;
    auto start = std::chrono::steady_clock::now();

    if (method == "random") {
        int count = getInt(args, "count", 10000);
        Rng rng(seed);
        result = runRandomSearch(problem, evaluator, rng, count);
        totalEvaluations = evaluator.getCount();
        runs = 1;
        configText = "count=" + std::to_string(count);
    } else if (method == "greedy") {
        result = runGreedy(problem, distance, evaluator);
        totalEvaluations = evaluator.getCount();
        runs = 1;
        configText = "nearest neighbour z kazdego miasta";
    } else if (method == "ea") {
        EAConfig config;
        config.popSize = getInt(args, "pop", config.popSize);
        config.generations = getInt(args, "gen", config.generations);
        config.px = getDouble(args, "px", config.px);
        config.pm = getDouble(args, "pm", config.pm);
        config.tourSize = getInt(args, "tour", config.tourSize);
        config.elitism = getInt(args, "elite", config.elitism);
        config.greedyInit = getDouble(args, "greedy-init", config.greedyInit);
        config.maxEvaluations = getInt(args, "max-evals", config.maxEvaluations);
        config.selection = parseSelection(getText(args, "sel", "tournament"));
        config.crossover = parseCrossover(getText(args, "cross", "ox"));
        config.mutation = parseMutation(getText(args, "mut", "inversion"));
        configText = describeConfig(config);

        for (int run = 0; run < runs; run++) {
            Rng rng(seed + run);
            evaluator.resetCount();
            EAResult ea = runEvolutionaryAlgorithm(problem, distance, evaluator, config, rng);
            totalEvaluations += evaluator.getCount();
            result.values.push_back(ea.best.fitness);
        }
    } else if (method == "sa") {
        SAConfig config;
        config.iterations = getInt(args, "iter", config.iterations);
        config.startTemperature = getDouble(args, "t0", config.startTemperature);
        config.startAcceptance = getDouble(args, "accept", config.startAcceptance);
        config.coolingRate = getDouble(args, "cooling", config.coolingRate);
        config.move = parseMutation(getText(args, "move", "inversion"));
        config.greedyStart = getInt(args, "greedy-start", 0) == 1;
        configText = describeConfig(config);

        for (int run = 0; run < runs; run++) {
            Rng rng(seed + run);
            evaluator.resetCount();
            SAResult sa = runSimulatedAnnealing(problem, distance, evaluator, config, rng);
            totalEvaluations += evaluator.getCount();
            result.values.push_back(sa.best.fitness);
        }
    } else {
        throw std::runtime_error("Nieznana metoda: " + method + " (dozwolone: random, greedy, ea, sa)");
    }

    auto end = std::chrono::steady_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    Stats stats = computeStats(result.values);
    configText += " packing=" + packing;

    std::cout << method << " | " << instance << " | " << configText << " | runs=" << runs << '\n'
              << "  best = " << stats.best << "  worst = " << stats.worst
              << "  avg = " << stats.avg << "  std = " << stats.std
              << "  ocen/run = " << totalEvaluations / runs
              << "  czas/run = " << timeMs / runs << " ms\n";

    if (outPath.has_parent_path()) {
        std::filesystem::create_directories(outPath.parent_path());
    }
    bool newFile = !std::filesystem::exists(outPath) || std::filesystem::file_size(outPath) == 0;

    std::ofstream csv(outPath, std::ios::app);
    if (!csv.is_open()) {
        throw std::runtime_error("Nie mozna otworzyc pliku: " + outPath.string());
    }
    csv << std::fixed << std::setprecision(2);
    if (newFile) {
        csv << "label;method;instance;config;runs;best;worst;avg;std;evaluations;time_ms\n";
    }
    csv << label << ';' << method << ';' << instance << ';' << configText << ';' << runs << ';'
        << stats.best << ';' << stats.worst << ';' << stats.avg << ';' << stats.std << ';'
        << totalEvaluations / runs << ';' << timeMs / runs << '\n';

    return 0;
}
