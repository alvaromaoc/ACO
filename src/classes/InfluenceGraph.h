//
// Created by alvaro on 27/2/22.
//

#ifndef INFLUENCEGRAPH_H
#define INFLUENCEGRAPH_H

#define UNDIRECTED true

#define THRESHOLD_TYPE 0
    #define THRESHOLD_UNIFORM 0
    #define THRESHOLD_HALF 1
    #define THRESHOLD_RAND 2

#include <vector>
#include <random>
#include "UniqueQueue.h"

class InfluenceGraph {
private:
    std::vector<double> thresholds;
    std::vector<std::vector<bool>> adjacencyMatrix;

    std::vector<int> individualSpreads;
    std::vector<std::vector<int>> distances;

    bool checkActive(std::vector<bool> actives, int node);
    void spreadTask(std::vector<bool> &actives, int node);
    int shortestPath(int u, int v, std::vector<bool> seen);
public:
    InfluenceGraph(); // Solo para la inicializacion en vectores;
    InfluenceGraph(int nodes, int seed);

    int numVertex();
    void setEdge(int node1, int node2);

    int shortestPath(int u, int v);
    void setPriorKnowledge();

    int spread(std::vector<bool> seed);
    int addToCurrentSpread(std::vector<bool> &actives, int currentSpread, int newNode);
    int spread(int u);
    int spreadParallel(std::vector<bool> seed);

    bool isNeighbour(int u, int v);
    double getThreshold(int u);
    void setThreshold(int u, double threshold);
};

#endif //INFLUENCEGRAPH_H
