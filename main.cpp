#include <exception>
#include <iomanip>
#include <iostream>

#include "ttp/CommandLine.h"
#include "ttp/Experiments.h"
#include "ttp/Tests.h"

int main(int argc, char* argv[]) {
    std::cout << std::fixed << std::setprecision(2);

    try {
        if (argc > 1) {
            return runSingleExperiment(argc, argv);
        }

        if (!testTinyInstance()) {
            std::cerr << "Funkcja oceny liczy zle, przerywam.\n";
            return 1;
        }
        if (!testOperators()) {
            std::cerr << "Operatory psuja trasy, przerywam.\n";
            return 1;
        }

        return runFullComparison();
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }
}
