#include "ttp/Config.h"

#include <sstream>

std::string describeConfig(const EAConfig& config) {
    std::string selection = "tournament";
    if (config.selection == SelectionType::Roulette) {
        selection = "roulette";
    }

    std::string crossover = "OX";
    if (config.crossover == CrossoverType::PMX) {
        crossover = "PMX";
    }

    std::string mutation = "inversion";
    if (config.mutation == MutationType::Swap) {
        mutation = "swap";
    }

    std::ostringstream text;
    text << "pop=" << config.popSize
         << " gen=" << config.generations;
    if (config.maxEvaluations > 0) {
        text << " maxEval=" << config.maxEvaluations;
    }
    text
         << " Px=" << config.px
         << " Pm=" << config.pm
         << " sel=" << selection;
    if (config.selection == SelectionType::Tournament) {
        text << " Tour=" << config.tourSize;
    }
    text << " elite=" << config.elitism
         << " cross=" << crossover
         << " mut=" << mutation
         << " greedyInit=" << config.greedyInit;
    return text.str();
}

std::string describeConfig(const SAConfig& config) {
    std::string move = "inversion";
    if (config.move == MutationType::Swap) {
        move = "swap";
    }

    std::ostringstream text;
    text << "iter=" << config.iterations;
    if (config.startTemperature > 0.0) {
        text << " T0=" << config.startTemperature;
    } else {
        text << " T0=auto(accept=" << config.startAcceptance << ")";
    }
    text << " cooling=" << config.coolingRate
         << " move=" << move;
    if (config.greedyStart) {
        text << " start=greedy";
    } else {
        text << " start=random";
    }
    return text.str();
}
