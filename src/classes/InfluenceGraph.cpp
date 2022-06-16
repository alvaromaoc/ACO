//
// Created by alvaro on 27/2/22.
//

#include "InfluenceGraph.h"

#include <iostream>

InfluenceGraph::InfluenceGraph() {}

InfluenceGraph::InfluenceGraph(int nodes, int seed) {    
    this->adjacencyMatrix = std::vector<std::vector<bool>>(nodes,std::vector<bool>(nodes,false));
    this->thresholds = std::vector<double>(nodes);

    std::default_random_engine generator = std::default_random_engine();
    generator.seed(seed);
    std::uniform_real_distribution<double> distribution = std::uniform_real_distribution<double>(0.0,1.0);
    for (int i = 0;i < nodes;++i) {
        switch (THRESHOLD_TYPE)
        {
            case THRESHOLD_UNIFORM:
                this->thresholds[i] = distribution(generator);
                break;
            case THRESHOLD_HALF:
                this->thresholds[i] = 0.5;
                break;
            case THRESHOLD_RAND:
                this->thresholds[i] = ((float) rand() / (RAND_MAX));
            default:
                break;
        }
    }

    this->individualSpreads = std::vector<int>(nodes,0);
    this->distances = std::vector<std::vector<int>>(nodes,std::vector<int>(nodes,-1));
}

bool InfluenceGraph::checkActive(std::vector<bool> actives, int node) {
    int incidents = 0;
    int activeIncidents = 0;
    for (int i = 0; i < this->adjacencyMatrix.size(); ++i) {
        if (this->adjacencyMatrix[i][node]) {
            ++incidents;
            if (actives[i]) {
                ++activeIncidents;
            }
        }
    }
    return float(activeIncidents)/float(incidents) >= this->thresholds[node];
}

void InfluenceGraph::setEdge(int node1, int node2) {
    this->adjacencyMatrix[node1][node2] = true; // node1 -> node2
    if (UNDIRECTED) {
        this->adjacencyMatrix[node2][node1] = true; // node2 -> node1
    }
}

int InfluenceGraph::numVertex() {
    return this->thresholds.size();
}

int InfluenceGraph::shortestPath(int u, int v, std::vector<bool> seen) {
    if (this->distances[u][v] != -1) {
        return this->distances[u][v];
    }
    if (u == v) {
        this->distances[u][v] = 0;
        return 0;
    }
    if (UNDIRECTED && this->distances[v][u] != -1) {
        this->distances[u][v] = this->distances[v][u];
        return this->distances[u][v];
    }
    seen[u] = true;
    int bestDistance = this->numVertex();
    for (int i = 0;i < adjacencyMatrix.size();++i) {
        if (adjacencyMatrix[u][i] && !seen[i]) {
            int distance = this->shortestPath(i,v,seen) + 1;
            if (distance < bestDistance) {
                bestDistance = distance;
            }
        }
    }
    this->distances[u][v] = bestDistance;
    return bestDistance;
}

int InfluenceGraph::shortestPath(int u, int v) {
    return this->shortestPath(u,v,std::vector<bool>(this->numVertex()));
}

void InfluenceGraph::setPriorKnowledge() {
    for (int i = 0;i < this->adjacencyMatrix.size();++i) {
        this->individualSpreads[i] = this->spread(i);
        for (int j = UNDIRECTED ? i : 0;j < this->adjacencyMatrix.size();++j) {
            this->shortestPath(i,j);
        }
    }
}

int InfluenceGraph::spread(std::vector<bool> actives) {
    int activesCount = 0;
    std::list<int> list;
    for (int i = 0;i < this->numVertex();++i) {
        if (actives[i]) {
            list.push_back(i);
            ++activesCount;
        }
    }
    unique_queue newActives(this->numVertex(), list);
    while (!newActives.empty()) {
        std::vector<bool> neighborsMask = this->adjacencyMatrix[newActives.front()];
        for (int i = 0; i < this->numVertex(); ++i) {
            if (neighborsMask[i] && !actives[i] && this->checkActive(actives, i)) {
                newActives.push(i);
                actives[i] = true;
                ++activesCount;
            }
        }
        newActives.pop();
    }
    return activesCount;
}

int InfluenceGraph::addToCurrentSpread(std::vector<bool> &actives, int currentSpread, int newNode) {
    unique_queue newActives(this->numVertex());
    if (!actives[newNode]) {
        actives[newNode] = true;
        ++currentSpread;
    }
    newActives.push(newNode);
    while (!newActives.empty()) {
        std::vector<bool> neighborsMask = this->adjacencyMatrix[newActives.front()];
        for (int i = 0; i < this->numVertex(); ++i) {
            if (neighborsMask[i] && !actives[i] && this->checkActive(actives, i)) {
                newActives.push(i);
                actives[i] = true;
                ++currentSpread;
            }
        }
        newActives.pop();
    }
    return currentSpread;
}

int InfluenceGraph::spread(int u) {
    if (this->individualSpreads[u] != 0) {
        return this->individualSpreads[u];
    } else {
        std::vector<bool> seed(this->numVertex(), false);
        seed[u] = true;
        this->individualSpreads[u] = this->spread(seed);
        return this->individualSpreads[u];
    }
}

int InfluenceGraph::spreadParallel(std::vector<bool> actives) {
    #pragma omp parallel
    #pragma omp single
    {
        for (int i = 0;i < this->numVertex();++i) {
            if (actives[i]) {
                spreadTask(actives, i);
            }
        }
        #pragma omp taskwait
    }
    int activesCount = 0;
    for (int i = 0;i < this->numVertex();++i) {
        if (actives[i]) {
            ++activesCount;
        }
    }
    return activesCount;
}

void InfluenceGraph::spreadTask(std::vector<bool> &actives, int node) {
    #pragma omp task shared(actives)
    {
        #pragma omp critical
        actives[node] = true;
        std::vector<bool> neighborsMask = this->adjacencyMatrix[node];
        for (int i = 0; i < this->numVertex(); ++i) {
            if (neighborsMask[i] && !actives[i] && this->checkActive(actives, i)) {
                this->spreadTask(actives, i);
            }
        }
    }
}

bool InfluenceGraph::isNeighbour(int u, int v) {
    return this->adjacencyMatrix[u][v];
}

double InfluenceGraph::getThreshold(int u) {
    return this->thresholds[u];
}

void InfluenceGraph::setThreshold(int u, double threshold) {
    this->thresholds[u] = threshold;
}