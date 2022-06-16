#include "Roulette.h"

int roulette(std::vector<int> elems, int startIndex, std::vector<double> scores, double total) {    
    double rndNumber = std::rand() / double(RAND_MAX);
    double offset = 0.0;
    int i = startIndex;
    for (i; i < elems.size(); ++i) {
        offset += scores[elems[i]]/total;
        if (rndNumber <= offset) break;
    }
    return i;
}

void roulette_shuffle(std::vector<int> &elems, std::vector<double> scores) {
    double total = 0;
    for (auto it = elems.begin();it != elems.end();++it) {
        total += scores[*it];
    }

    for (int t = 0;t < elems.size();++t)
    {
        int pick = roulette(elems,t,scores,total);
        total -= scores[elems[pick]];
        int aux = elems[t];
        elems[t] = elems[pick];
        elems[pick] = aux;
    }
}