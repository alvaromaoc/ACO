#include <vector>
#include <random>

#ifndef POPULATION_H
#define POPULATION_H

class Population {

private:
    int score;
    std::vector<int> nodes;
    std::vector<bool> nodesMask;
    std::vector<bool> influenced;
public:

    Population();
    Population(int numNodes);
    ~Population();

    bool operator>(const Population &p) const {
        return this->score > p.score;
    }

    bool operator<(const Population &p) const {
        return this->score < p.score;
    }

    bool operator==(const Population &p) const {
        return this->score == p.score;
    }
    
    static Population random(int numNodes, int k) {
        Population p(numNodes);
        p.nodes = std::vector<int>(k);
        for (int i = 0;i < k;++i) {
            int random = std::rand() % numNodes;
            for (int j = 0;j < numNodes;++j) {
                if (!p.nodesMask[j] && j >= random) {
                    p.nodes[i] = j;
                    p.nodesMask[j] = true;
                    break;
                }
            }
        }
        return p;
    }

    void replaceNode(int nodesPos, int newNode);

    bool hasNode(int u);

    bool isInfluenced(int u);

    void addNode(int u);

    std::size_t size();

    int getPos(int pos);

    int getSpread();

    void setSpread(int spread);

    void setInfluenced(std::vector<bool> influenced);

    std::vector<bool> getInfluenced();

    std::vector<bool> getSeed();

    std::vector<int> getNodes();

    bool reached();

    // Los nodos deben ser los mismos, lo que puede cambiar es el orden
    void setNodes(std::vector<int> newNodes);
};

#endif //POPULATION_H