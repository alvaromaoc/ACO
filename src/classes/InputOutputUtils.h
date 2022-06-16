#include "Algorithms.h"
#include "AppConfig.h"
#include "Algorithm.h"
#include <string>
#include <fstream>
#include <sstream>

#ifndef INPUT_OUTPUT_UTILS_H
#define INPUT_OUTPUT_UTILS_H

AppConfig setAppConfig(int argc, char* argv[]);

std::vector<InfluenceGraph> readNetwork(std::string filePath, int graphIterationNumber);

void writeNetwork(std::vector<InfluenceGraph> graphs, std::string folder);

void printInfo(AppConfig &appConfig, int timePriorKnowledge);

#endif //INPUT_OUTPUT_UTILS_H