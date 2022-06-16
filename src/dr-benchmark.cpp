#include "classes/InputOutputUtils.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int k[] = {25,35};
    double initialValue = 0.5;
    double endValue = 1.01;
    double step = 0.01;
    int stopConditionNumber = 10000;
    std::vector<InfluenceGraph> g = readNetwork(argv[1],1);
    for (int i = 0;i < 2;++i) {
        std::cout << "k" << k[i] << "=[";
        for (double value = initialValue;value < endValue;value += step) {
            double acc = 0.0;
            #pragma omp parallel for reduction (+:acc)
            for (int j = 0;j < 100;++j) {
                acc += aco(g[0],k[i],stopConditionNumber,value).getSpread();
            }
            acc /= 100;
            std::cout << value << "," << acc;
            if (value + step < endValue) {
                std::cout << ";";
            } else {
                std::cout << "]";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}