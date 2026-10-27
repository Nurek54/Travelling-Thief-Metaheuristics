#pragma once

#include "ttp/Distance.h"
#include "ttp/Individual.h"
#include "ttp/Problem.h"

void packItems(const Problem& problem, const Distance& distance, Individual& individual, bool profitCheck = false);
