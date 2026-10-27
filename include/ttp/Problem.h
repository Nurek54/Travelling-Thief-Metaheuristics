#pragma once

#include <string>
#include <vector>

#include "ttp/Item.h"

struct City {
    double x = 0.0;
    double y = 0.0;
};

struct Problem {
    std::string name;

    int numCities = 0;
    int numItems = 0;
    int capacity = 0;

    double minSpeed = 0.0;
    double maxSpeed = 0.0;
    double rentingRatio = 0.0;

    std::vector<City> cities;
    std::vector<Item> items;
    std::vector<std::vector<int>> itemsByCity;
};
