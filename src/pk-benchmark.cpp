#include "classes/InputOutputUtils.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int times = 100;
    #pragma omp parallel for
    for (int i = 0;i < times;++i) {
        std::vector<InfluenceGraph> g = readNetwork(argv[1],1);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        g[0].setPriorKnowledge();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int time = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
        #pragma omp critical
        {
            std::cout << time << ",";
        }
    }
}