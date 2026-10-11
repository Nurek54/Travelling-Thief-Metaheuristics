// Implementacja Loader.h
#include "ttp/Loader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

enum class Section {
    Header,
    Cities,
    Items
};

// Usuwa biale znaki (spacje, tabulatory) z poczatku i konca tekstu.
std::string trim(const std::string& text) {
    const char* whitespace = " \t";
    const auto begin = text.find_first_not_of(whitespace);
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = text.find_last_not_of(whitespace);
    return text.substr(begin, end - begin + 1);
}

// Linia naglowka w formacie "KLUCZ: wartosc".
void parseHeaderLine(const std::string& line, Problem& problem) {
    const auto colon = line.find(':');
    if (colon == std::string::npos) {
        return;
    }

    const std::string key = trim(line.substr(0, colon));
    const std::string value = trim(line.substr(colon + 1));

    if (key == "PROBLEM NAME") {
        problem.name = value;
    } else if (key == "DIMENSION") {
        problem.numCities = std::stoi(value);
    } else if (key == "NUMBER OF ITEMS") {
        problem.numItems = std::stoi(value);
    } else if (key == "CAPACITY OF KNAPSACK") {
        problem.capacity = std::stoi(value);
    } else if (key == "MIN SPEED") {
        problem.minSpeed = std::stod(value);
    } else if (key == "MAX SPEED") {
        problem.maxSpeed = std::stod(value);
    } else if (key == "RENTING RATIO") {
        problem.rentingRatio = std::stod(value);
    }
    // KNAPSACK DATA TYPE i EDGE_WEIGHT_TYPE pomijamy, nie wplywaja na obliczenia.
}

// Linia miasta w formacie "id x y".
void parseCityLine(const std::string& line, Problem& problem) {
    std::istringstream iss(line);
    int id = 0;
    City city;
    if (!(iss >> id >> city.x >> city.y)) {
        throw std::runtime_error("Bledna linia miasta: " + line);
    }
    problem.cities.push_back(city);
}

// Linia przedmiotu w formacie "id profit weight city". Numeracja w pliku od 1, w kodzie od 0.
void parseItemLine(const std::string& line, Problem& problem) {
    std::istringstream iss(line);
    Item item;
    if (!(iss >> item.id >> item.profit >> item.weight >> item.city)) {
        throw std::runtime_error("Bledna linia przedmiotu: " + line);
    }
    item.id -= 1;
    item.city -= 1;
    problem.items.push_back(item);
}

// Sprawdza, czy wczytane dane zgadzaja sie z naglowkiem.
void validate(const Problem& problem, const std::string& path) {
    if (static_cast<int>(problem.cities.size()) != problem.numCities) {
        throw std::runtime_error("Liczba miast nie zgadza sie z DIMENSION w pliku: " + path);
    }
    if (static_cast<int>(problem.items.size()) != problem.numItems) {
        throw std::runtime_error("Liczba przedmiotow nie zgadza sie z NUMBER OF ITEMS w pliku: " + path);
    }
    if (problem.capacity <= 0) {
        throw std::runtime_error("Nieprawidlowa pojemnosc plecaka w pliku: " + path);
    }
    if (problem.minSpeed <= 0.0 || problem.minSpeed >= problem.maxSpeed) {
        throw std::runtime_error("Nieprawidlowe MIN/MAX SPEED w pliku: " + path);
    }
    for (const Item& item : problem.items) {
        if (item.city < 0 || item.city >= problem.numCities) {
            throw std::runtime_error("Przedmiot " + std::to_string(item.id + 1) +
                                     " ma nieprawidlowe miasto w pliku: " + path);
        }
    }
}

// Dla kazdego miasta lista indeksow przedmiotow, ktore w nim leza.
void buildItemsByCity(Problem& problem) {
    problem.itemsByCity.assign(problem.numCities, {});
    for (const Item& item : problem.items) {
        problem.itemsByCity[item.city].push_back(item.id);
    }
}

} // namespace

Problem loadProblem(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Nie mozna otworzyc pliku: " + path);
    }

    Problem problem;
    Section section = Section::Header;
    std::string line;

    while (std::getline(file, line)) {
        // Usun '\r' z konca (pliki maja windowsowe konce linii)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Pomin puste linie
        if (trim(line).empty()) {
            continue;
        }

        // Przelaczanie sekcji (te linie same nie sa danymi)
        if (line.starts_with("NODE_COORD_SECTION")) {
            section = Section::Cities;
            continue;
        }
        if (line.starts_with("ITEMS SECTION")) {
            section = Section::Items;
            continue;
        }

        // Linia z danymi: co z nia zrobic, zalezy od sekcji
        switch (section) {
            case Section::Header:
                parseHeaderLine(line, problem);
                break;
            case Section::Cities:
                parseCityLine(line, problem);
                break;
            case Section::Items:
                parseItemLine(line, problem);
                break;
        }
    }

    validate(problem, path);
    buildItemsByCity(problem);

    return problem;
}
