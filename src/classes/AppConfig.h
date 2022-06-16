#include <vector>
#include <filesystem>
#include "Algorithm.h"

#ifndef APPCONFIG_H
#define APPCONFIG_H

namespace fs = std::filesystem;

class AppConfig {
public:
    std::string folder;

    std::string networkPath;
    std::vector<Algorithm> algorithms;

    bool priorKnowledge;
    bool printSeed;
    bool printVariance;
    bool checkSeed;
    bool exportNetwork;
    bool stopOnReach;

    int from;
    int to;
    int stopConditionNumber;
    int graphIterationNumber;

    std::string error;

    AppConfig();
};

#endif //APPCONFIG_H