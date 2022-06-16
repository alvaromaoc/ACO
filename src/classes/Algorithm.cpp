#include "Algorithm.h"

Algorithm::Algorithm(
    std::string folder,
    Population (*algo)(InfluenceGraph, int, int),
    std::string name,
    std::string runs
) {
    this->algo = algo;
    this->name = name;
    if (!runs.empty()) {
        this->runs = std::stoi(runs);
    } else {
        this->runs = 1;
    }
}

void Algorithm::setExperiment(
    int from,
    int to,
    bool stopOnReach,
    bool printSeed,
    bool printVariance,
    int graphIterationNumber,
    std::string folder
) {
    this->from = from;
    this->to = to;
    this->stopOnReach = stopOnReach;
    this->printSeed = printSeed;
    this->printVariance = printVariance;

    this->graphsRunsCounter = std::vector<int>(graphIterationNumber,0);
    this->graphsReached = std::vector<bool>(graphIterationNumber,true);
    this->graphsReachedK = std::vector<int>(graphIterationNumber, this->to+1);

    this->spreads = std::ofstream("./Data/" + folder + "/spreads_" + this->name + ".csv");
    this->times = std::ofstream("./Data/" + folder + "/times_" + this->name + ".csv");
    if (this->printSeed) {
        this->seeds = std::ofstream("./Data/" + folder + "/seeds_" + this->name + ".csv");
    }

    int kSize = this->to-this->from + 1;
    this->timeIts = getMat<int>(kSize, graphIterationNumber, this->runs);
    this->results = getMat<Population>(kSize, graphIterationNumber, this->runs);
    this->numGraphsFinishedInK = std::vector<int>(kSize,0);
    
    this->spreads << "k";
    this->times << "k";
    this->seeds << "k";
    for (int gi = 0;gi < graphIterationNumber;++gi) {
        this->seeds << ",g" << gi+1;
        for (int j = 0;j < this->runs;++j) {
            this->spreads << ",g" << gi+1 << "r" << j+1;
            this->times << ",g" << gi+1 << "r" << j+1;
        }
    }
    this->spreads << std::endl;
    this->times << std::endl;
    this->seeds << std::endl;
}

bool Algorithm::setResult(int k, int g, int r, Population result, int time) {
    int index = k-this->from;
    this->results[index][g][r] = result;
    this->timeIts[index][g][r] = time;
    this->graphsReached[g] = this->graphsReached[g] && result.getSpread() == result.getSeed().size();
    ++this->graphsRunsCounter[g];
    if (this->graphsRunsCounter[g] == this->runs) {
        this->graphsRunsCounter[g] = 0;

        ++numGraphsFinishedInK[index];
        if (numGraphsFinishedInK[index] == graphsReached.size()) {
            this->newSize(k);
        }
        
        if (graphsReached[g]) {
            this->graphsReachedK[g] = k;
            if (this->stopOnReach) {
                for (int i = k+1;i < numGraphsFinishedInK.size();++i) {
                    ++numGraphsFinishedInK[i-this->from];
                    if (numGraphsFinishedInK[i-this->from] == graphsReached.size() && *std::max_element(graphsReachedK.begin(), graphsReachedK.end()) >= i) {
                        this->newSize(i);
                    }
                }
            }
            return !this->stopOnReach;
        } else {
            this->graphsReached[g] = true;
            return true;
        }
    }
    return false;
}

void Algorithm::newSize(int k) {
    spreads << k;
    times << k;
    if (this->printSeed) {
        seeds << k;
    }
    int index = k-this->from;
    for (int g = 0;g < results[index].size();++g) {
        if (this->graphsReachedK[g] >= k || !this->stopOnReach) {
            for (int r = 0;r < results[index][g].size();++r) {
                spreads << "," << results[index][g][r].getSpread();
                times << "," << timeIts[index][g][r];
            }
        } else {
            for (int r = 0;r < results[index][g].size();++r) {
                spreads << ",";
                times << ",";
            }
        }
    }
    spreads << std::endl;
    times << std::endl;
    if (this->printSeed) {
        for (int i = 0;i < results[index].size();++i) {
            seeds << ",";
            if (this->graphsReachedK[i] >= k || !this->stopOnReach) {
                seeds << seedToString((*std::max_element(results[index][i].begin(), results[index][i].end())).getSeed());
            }
        }
        seeds << std::endl;
    }
}

void Algorithm::getGeneralRows(std::ofstream &general, int k) {
    int lastKReached = *std::max_element(graphsReachedK.begin(), graphsReachedK.end());
    int index = k-this->from;
    if (lastKReached >= k || !this->stopOnReach) {
        int sumSpread = 0;
        int sumTime = 0;
        int maxSpread = 0;
        int minSpread = results[index][0][0].getSeed().size();

        for (int g = 0;g < results[index].size();++g) {
            for (int r = 0;r < results[index][g].size();++r) {
                int kIndex;
                if (this->graphsReachedK[g] >= k || !this->stopOnReach) {
                    kIndex = index;
                } else { 
                    kIndex = graphsReachedK[g]-this->from;
                }
                sumSpread += results[kIndex][g][r].getSpread();
                sumTime += timeIts[kIndex][g][r];
                if (results[kIndex][g][r].getSpread() > maxSpread) { maxSpread = results[kIndex][g][r].getSpread(); }
                if (results[kIndex][g][r].getSpread() < minSpread) { minSpread = results[kIndex][g][r].getSpread(); }
            }
        }
        int numResults = results[index].size()*results[index][0].size();
        general << "," << double(sumSpread)/double(numResults) << "," << double(sumTime)/double(numResults);
        if (this->printVariance) {
            general << "," << minSpread << "," << maxSpread;
        }
    } else {
        general << ",,";
        if (this->printVariance) {
            general << ",,";
        }
    }
}