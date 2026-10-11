// Wczytywanie pliku .ttp do struktury Problem.
#pragma once

#include <string>
#include "ttp/Problem.h"

Problem loadProblem(const std::string& path);
