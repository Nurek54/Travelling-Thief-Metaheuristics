// Implementacja Logger.h
#include "ttp/Logger.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

void saveHistoryCsv(const std::string& path, const std::vector<GenerationStats>& history) {
    // Tworzymy folder, jesli go nie ma (np. results/history)
    std::filesystem::path folder = std::filesystem::path(path).parent_path();
    if (!folder.empty()) {
        std::filesystem::create_directories(folder);
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Nie mozna utworzyc pliku: " + path);
    }

    file << std::fixed << std::setprecision(2);
    file << "generation;best;avg;worst\n";
    for (const GenerationStats& stats : history) {
        file << stats.generation << ';'
             << stats.best << ';'
             << stats.avg << ';'
             << stats.worst << '\n';
    }
}
