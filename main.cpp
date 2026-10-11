#include <exception>
#include <iostream>
#include <string>

#include "ttp/Loader.h"

int main() {
    const std::string path = std::string(TTP_DATA_DIR) + "/easy/a280_n279_uncorr_01.ttp";

    try {
        const Problem problem = loadProblem(path);

        // TYMCZASOWE: kontrola wczytania
        std::cout << "Nazwa:        " << problem.name << '\n'
                  << "Miasta:       " << problem.numCities << " (wczytano " << problem.cities.size() << ")\n"
                  << "Przedmioty:   " << problem.numItems << " (wczytano " << problem.items.size() << ")\n"
                  << "Pojemnosc:    " << problem.capacity << '\n'
                  << "vmin / vmax:  " << problem.minSpeed << " / " << problem.maxSpeed << '\n'
                  << "Renting:      " << problem.rentingRatio << '\n';

        const City& firstCity = problem.cities.front();
        const City& lastCity = problem.cities.back();
        std::cout << "Miasto 1:     " << firstCity.x << " " << firstCity.y << '\n'
                  << "Miasto 280:   " << lastCity.x << " " << lastCity.y << '\n';

        const Item& firstItem = problem.items.front();
        const Item& lastItem = problem.items.back();
        std::cout << "Pierwszy prz: id=" << firstItem.id << " profit=" << firstItem.profit
                  << " weight=" << firstItem.weight << " city=" << firstItem.city << '\n'
                  << "Ostatni prz:  id=" << lastItem.id << " profit=" << lastItem.profit
                  << " weight=" << lastItem.weight << " city=" << lastItem.city << '\n';

        std::cout << "itemsByCity[0].size() = " << problem.itemsByCity[0].size() << " (oczekiwane 0)\n"
                  << "itemsByCity[1].size() = " << problem.itemsByCity[1].size() << " (oczekiwane 1)\n";
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
