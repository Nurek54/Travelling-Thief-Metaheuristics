#pragma once

#include <string>

enum class SelectionType {
    Tournament,
    Roulette
};

enum class CrossoverType {
    OX,
    PMX
};

enum class MutationType {
    Swap,
    Inversion
};

struct EAConfig {
    int popSize = 100;
    int generations = 100;
    double px = 0.7;
    double pm = 0.1;
    int tourSize = 5;
    int elitism = 1;
    int maxEvaluations = 0;
    double greedyInit = 0.0;

    SelectionType selection = SelectionType::Tournament;
    CrossoverType crossover = CrossoverType::OX;
    MutationType mutation = MutationType::Inversion;
};

struct SAConfig {
    int iterations = 10000;
    double startTemperature = 0.0;
    double startAcceptance = 0.5;
    double coolingRate = 0.999;
    MutationType move = MutationType::Inversion;
    bool greedyStart = false;
    int logEvery = 100;
};

std::string describeConfig(const EAConfig& config);
std::string describeConfig(const SAConfig& config);
