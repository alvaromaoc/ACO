#include <vector>
#include <random>

#ifndef ROULETTE_H
#define ROULETTE_H

int roulette(std::vector<int> elems, int startIndex, std::vector<double> scores, double total);

void roulette_shuffle(std::vector<int> &elems, std::vector<double> scores);

#endif //ROULETTE_H