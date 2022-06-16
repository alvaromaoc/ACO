#include "classes/InputOutputUtils.h"
#include <iostream>

AppConfig appConfig;

void newIt(
    int k, int a, int g, int r,
    std::vector<InfluenceGraph> &graphs,
    std::vector<Algorithm> &experiments
) {
    #pragma omp task firstprivate(k,a,g,r) shared(experiments,graphs)
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        Population result = appConfig.algorithms[a].algo(graphs[g],k,appConfig.stopConditionNumber);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int time = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();

        if (appConfig.checkSeed) {
            int realSpread = graphs[g].spread(result.getSeed());
            if (realSpread != result.getSpread()) {
                std::cout << "ERROR: Incorrect spread calculation" << std::endl;
                std::cout << "\tAlgorithm: " << appConfig.algorithms[a].name << std::endl;
                std::cout << "\tReal spread: " << realSpread << std::endl;
                std::cout << "\tCalculated spread: " << result.getSpread() << std::endl;
            }
        }

        bool ended;
        #pragma omp critical
        {
            ended = experiments[a].setResult(k,g,r,result,time);
        }
        if (ended && k < appConfig.to) {
            for (int r = 0;r < appConfig.algorithms[a].runs;++r) {
                newIt(k+1,a,g,r,graphs,experiments);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    appConfig = setAppConfig(argc, argv);
    if (!appConfig.error.empty()) {
        std::cout << appConfig.error << std::endl;
        return 0;
    }

    int timePriorKnowledge = -1;
    std::vector<InfluenceGraph> graphs = readNetwork(appConfig.networkPath, appConfig.graphIterationNumber);
    if (appConfig.priorKnowledge) {
        timePriorKnowledge = 0;
        #pragma omp parallel for reduction(+:timePriorKnowledge) shared(graphs)
        for (int i = 0;i < graphs.size();++i) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            graphs[i].setPriorKnowledge();
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            timePriorKnowledge += std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
        }
        timePriorKnowledge /= graphs.size();
    }

    printInfo(appConfig, timePriorKnowledge);

    if (appConfig.exportNetwork) {
        writeNetwork(graphs, appConfig.folder);
    }

    for (auto it = appConfig.algorithms.begin();it != appConfig.algorithms.end();++it) {
        it->setExperiment(
            appConfig.from,
            appConfig.to,
            appConfig.stopOnReach,
            appConfig.printSeed,
            appConfig.printVariance,
            appConfig.graphIterationNumber,
            appConfig.folder
        );
    }

    #pragma omp parallel
    #pragma omp single
    {
        #pragma omp taskgroup
        {
            for (int a = 0;a < appConfig.algorithms.size();++a) {
                for (int g = 0;g < graphs.size();++g) {
                    for (int r = 0;r < appConfig.algorithms[a].runs;++r) {
                        newIt(appConfig.from, a, g, r, graphs, appConfig.algorithms);
                    }
                }
            }
        }
    }

    /* --- Extracción datos generales --- */
    std::ofstream general("./Data/" + appConfig.folder + "/general.csv");
    int algColumns = 2 + (appConfig.printVariance ? 2 : 0);
    general << "k";
    for (int i = 0;i < appConfig.algorithms.size()*algColumns;++i) {
        int iAlg = i/algColumns;
        general << "," << appConfig.algorithms[iAlg].name;
    }
    general << std::endl;
    general << "k";
    for (int i = 0;i < appConfig.algorithms.size();++i) {
        general << ",Spread,Time (ms)";
        if (appConfig.printVariance) {
            general << ",Min spread,Max spread";
        }
    }
    general << std::endl;

    int reachedK = 0;
    for (int i = 0;i < appConfig.algorithms.size();++i) {
        int reachedKAlgo = *max_element(appConfig.algorithms[i].graphsReachedK.begin(),appConfig.algorithms[i].graphsReachedK.end());
        if (reachedKAlgo > reachedK) {
            reachedK = reachedKAlgo;
        }
    }
    for (int k = appConfig.from; k <= appConfig.to && k <= reachedK; ++k) {
        general << k;
        for (int i = 0;i < appConfig.algorithms.size();++i) {
            appConfig.algorithms[i].getGeneralRows(general,k);
        }
        general << std::endl;
    }

    return 0;
}