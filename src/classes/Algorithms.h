#include "InfluenceGraph.h"
#include <vector>
#include <algorithm>
#include <set>
#include "Population.h"
#include "Roulette.h"

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

struct secondPairLessEqual {
    bool operator()(std::pair<int,int> a, std::pair<int,int> b) {
        return a.second <= b.second;
    }
};

struct greaterScore {
    InfluenceGraph* g;
    greaterScore(InfluenceGraph g) {
        this->g = &g;
    }
    bool operator()(int a, int b) {
        return g->spread(a) > g->spread(b);
    }
};

struct lessScore {
    InfluenceGraph* g;
    lessScore(InfluenceGraph g) {
        this->g = &g;
    }
    bool operator()(int a, int b) {
        return g->spread(a) < g->spread(b);
    }
};

/* ----- GREEDY ALGORITHM -----*/

/*
 * Maximizing the spread of influence through a social network
 * https://doi.org/10.1145/956750.956769
 */

Population greedy(InfluenceGraph g, int k, int stopConditionNumber);

/* ----- CELF ALGORITHM -----*/

/*
 * Cost-effective outbreak detection in networks
 * https://doi.org/10.1145/1281192.1281239
 */

Population celf(InfluenceGraph g, int k, int stopConditionNumber);

/* ----- GENETIC ALGORITHM -----*/

/*
 * Maximizing influence in a social network: Improved results using a genetic algorithm
 * https://doi.org/10.1016/j.physa.2017.02.067
 */

#define M 10
#define N 10 // Must be greater or equal than M
#define CP 0.2
#define MP 0.8

void crossover(Population &p1, Population &p2, InfluenceGraph g);

std::vector<int> IDH(Population p1, InfluenceGraph g);

void mutation(Population &p, InfluenceGraph &g);

Population genetic(InfluenceGraph g, int k, int stopConditionNumber);

/* ----- ACO ALGORITHM -----*/

/*
 * Adding Negative Learning to Ant Colony Optimization: A Comprehensive Study
 * https://doi.org/10.3390/math9040361
 */

#define N_ANTS 10
#define LEARNING_RATE 0.1
#define TAU_MAX 0.999
#define TAU_MIN 0.001
#define DETERMINISTIC_RATE 0.925

Population genetateAcoSolution(InfluenceGraph g, int k, std::vector<double> scores, double deterministicRate);

Population aco(InfluenceGraph g, int k, int stopConditionNumber);

Population aco(InfluenceGraph g, int k, int stopConditionNumber, double deterministicRate);

#endif //ALGORITHMS_H