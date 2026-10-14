// Implementacja Config.h
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
         << " gen=" << config.generations
         << " Px=" << config.px
         << " Pm=" << config.pm
         << " sel=" << selection;
    if (config.selection == SelectionType::Tournament) {
        text << " Tour=" << config.tourSize;
    }
    text << " elite=" << config.elitism
         << " cross=" << crossover
         << " mut=" << mutation;
    return text.str();
}
