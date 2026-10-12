// Generator losowy z seedem (mt19937). Jeden obiekt na uruchomienie algorytmu.
#pragma once

#include <cstdint>
#include <random>

class Rng {
public:
    explicit Rng(std::uint32_t seed) : seed_(seed), engine_(seed) {}

    // Losowa liczba calkowita z przedzialu [min, max] (oba konce wlacznie).
    int uniformInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine_);
    }

    // Losowa liczba rzeczywista z przedzialu [0, 1).
    double uniformReal() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(engine_);
    }

    // Dostep do silnika, np. dla std::shuffle.
    std::mt19937& engine() { return engine_; }

    std::uint32_t seed() const { return seed_; }

private:
    std::uint32_t seed_;
    std::mt19937 engine_;
};
