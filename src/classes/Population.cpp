#include "Population.h"

Population::Population() {
    this->score = 0;
    this->nodes = std::vector<int>();
}

Population::Population(int numNodes) {
    this->score = 0;
    this->nodes = std::vector<int>();
    this->nodesMask = std::vector<bool>(numNodes,false);
    this->influenced = std::vector<bool>(numNodes,false);
}

Population::~Population() {}

void Population::replaceNode(int nodesPos, int newNode) {
    this->nodesMask[this->nodes[nodesPos]] = false;
    this->nodesMask[newNode] = true;
    this->nodes[nodesPos] = newNode;
}

bool Population::hasNode(int u) {
    return this->nodesMask[u];
}

bool Population::isInfluenced(int u) {
    return this->influenced[u];
}

void Population::addNode(int u) {
    this->nodes.push_back(u);
    this->nodesMask[u] = true;
}

std::size_t Population::size() {
    return this->nodes.size();
}

int Population::getPos(int pos) {
    return this->nodes[pos];
}

int Population::getSpread() {
    return this->score;
}

void Population::setSpread(int spread) {
    this->score = spread;
}

void Population::setInfluenced(std::vector<bool> influenced) {
    this->influenced = influenced;
}

std::vector<bool> Population::getInfluenced() {
    return this->influenced;
}

std::vector<bool> Population::getSeed() {
    return this->nodesMask;
}

std::vector<int> Population::getNodes() {
    return this->nodes;
}

bool Population::reached() {
    return this->score == this->nodesMask.size();
}

// Los nodos deben ser los mismos, lo que puede cambiar es el orden
void Population::setNodes(std::vector<int> newNodes) {
    this->nodes = newNodes;
}