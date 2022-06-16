#include "AppConfig.h"

AppConfig::AppConfig() {
    this->folder = std::to_string(time(NULL));
    fs::create_directories("./Data/" + folder);

    this->error = "";
    this->networkPath = "";

    this->printSeed = false;
    this->printVariance = false;
    this->priorKnowledge = false;
    this->checkSeed = false;
    this->exportNetwork = false;
    this->stopOnReach = false;

    this->from = 1;
    this->to = 55;
    this->stopConditionNumber = 10000;
    this->graphIterationNumber = 1;
}