// Krzyzowanie: OX, PMX. Kazde zwraca jednego potomka.
// Zeby dostac dwoch potomkow, wywolaj drugi raz z zamienionymi rodzicami.
#pragma once

#include <vector>

#include "ttp/Individual.h"
#include "ttp/Rng.h"

// OX (Order Crossover), wersja z podanymi punktami ciecia a <= b.
// 1. Fragment [a, b] kopiujemy z rodzica 1 na te same pozycje.
// 2. Pozostale pozycje (od lewej) wypelniamy miastami z rodzica 2 w jego kolejnosci,
//    pomijajac te, ktore juz sa w dziecku.
std::vector<int> orderCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2, int a, int b);

// PMX (Partially Mapped Crossover), wersja z podanymi punktami ciecia a <= b.
// 1. Fragment [a, b] kopiujemy z rodzica 1.
// 2. Pozostale pozycje bierzemy z rodzica 2. Jesli miasto juz jest we fragmencie,
//    zamieniamy je wedlug mapowania rodzic1 <-> rodzic2 z tego fragmentu.
std::vector<int> pmxCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2, int a, int b);

// Wersje dla osobnikow z losowymi punktami ciecia.
Individual crossoverOX(const Individual& parent1, const Individual& parent2, Rng& rng);
Individual crossoverPMX(const Individual& parent1, const Individual& parent2, Rng& rng);
