#include <vector>
#include <fstream>
#include <algorithm>
#include "InfluenceGraph.h"
#include "Population.h"

#ifndef ALGORITHM_H
#define ALGORITHM_H

template<typename T>
using Mat3D = std::vector<std::vector<std::vector<T>>>;

template<typename T>
Mat3D<T> getMat(int a, int b, int c) {
    return std::vector<std::vector<std::vector<T>>>(a, std::vector<std::vector<T>>(b, std::vector<T>(c)));
}

class Algorithm {
public:
    Population (*algo)(InfluenceGraph, int, int);
    std::string name;
    int runs;

    std::ofstream spreads;
    std::ofstream times;
    std::ofstream seeds;

    Mat3D<int> timeIts;
    Mat3D<Population> results;

    std::vector<int> graphsRunsCounter;
    std::vector<bool> graphsReached;
    std::vector<int> graphsReachedK;
    std::vector<int> numGraphsFinishedInK;

    int from;
    int to;
    bool stopOnReach;
    bool printSeed;
    bool printVariance;

    static const std::string seedToString(std::vector<bool> seed) {
        std::string finalString = "";
        for (int i = 0;i < seed.size();++i) {
            if (seed[i]) {
                if (!finalString.empty()) {
                    finalString += " ";
                }
                finalString += std::to_string(i);
            }
        }
        return "[" + finalString + "]";
    }

    Algorithm(
        std::string folder,
        Population (*algo)(InfluenceGraph, int, int),
        std::string name,
        std::string runs
    );

    void setExperiment(
        int from,
        int to,
        bool stopOnReach,
        bool printSeed,
        bool printVariance,
        int graphIterationNumber,
        std::string folder
    );

    bool setResult(int k, int g, int r, Population result, int time);

    void newSize(int k);

    void getGeneralRows(std::ofstream &general, int k);
};

#endif //ALGORITHM_H