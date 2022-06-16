#include "Algorithms.h"

/* ----- GREEDY ALGORITHM -----*/

/*
 * Maximizing the spread of influence through a social network
 * https://doi.org/10.1145/956750.956769
 */

Population greedy(InfluenceGraph g, int k, int stopConditionNumber){
    Population sol(g.numVertex());
    for(int i = 0; i < k; ++i) {
        int bestNode = 0;
        int bestSpread = 0;
        std::vector<bool> bestInfluenced;
        for (int j = 0; j < g.numVertex(); ++j) {
            if (!sol.isInfluenced(j)) {
                std::vector<bool> influenced(sol.getInfluenced());
                int score = g.addToCurrentSpread(influenced,sol.getSpread(),j);
                if (score > bestSpread) {
                    bestNode = j;
                    bestSpread = score;
                    bestInfluenced = influenced;
                }
            }
        }
        sol.addNode(bestNode);
        sol.setSpread(bestSpread);
        sol.setInfluenced(bestInfluenced);

        if (sol.getSpread() == g.numVertex()) {
            break;
        }
    }
    
    return sol;
}

/* ----- CELF ALGORITHM -----*/

/*
 * Cost-effective outbreak detection in networks
 * https://doi.org/10.1145/1281192.1281239
 */

Population celf(InfluenceGraph g, int k, int stopConditionNumber) {
    std::vector<std::pair<int,int>> nodes(g.numVertex());
    for (int i = 0; i < g.numVertex(); ++i) {
        nodes[i] = std::make_pair(i, g.spread(i));
    }
    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, secondPairLessEqual > queue(nodes.begin(), nodes.end());

    Population sol(g.numVertex());
    sol.addNode(queue.top().first);
    sol.setSpread(queue.top().second);
    std::vector<bool> influenced(g.numVertex(),false);
    g.addToCurrentSpread(influenced, sol.getSpread(), queue.top().first);
    sol.setInfluenced(influenced);

    queue.pop();
    
    for (int i = 1; i < k; ++i) {

        bool check = false;
        
        std::vector<bool> influenced;
        while (!check) {

            std::pair<int,int> current = queue.top();
            queue.pop();
            influenced = std::vector<bool>(sol.getInfluenced());
            current.second = g.addToCurrentSpread(influenced, sol.getSpread(), current.first) - sol.getSpread();
            queue.push(current);

            check = queue.top().first == current.first;
        }

        sol.setSpread(sol.getSpread() + queue.top().second);
        sol.addNode(queue.top().first);
        sol.setInfluenced(influenced);
        queue.pop();

        if (sol.getSpread() == g.numVertex()) {
            break;
        }
    }

    return sol;
}

/* ----- GENETIC ALGORITHM -----*/

/*
 * Maximizing influence in a social network: Improved results using a genetic algorithm
 * https://doi.org/10.1016/j.physa.2017.02.067
 */

void crossover(Population &p1, Population &p2, InfluenceGraph g) {
    std::vector<int> p1Nodes = p1.getNodes();
    std::sort(p1Nodes.begin(),p1Nodes.end(), greaterScore(g));
    p1.setNodes(p1Nodes);

    std::vector<int> p2Nodes = p2.getNodes();
    std::sort(p2Nodes.begin(),p2Nodes.end(), lessScore(g));
    p2.setNodes(p2Nodes);

    int f = rand() % (p1.size() + 1);
    for (int i = 0;i < f;++i) {
        int v = p1.getPos(i), u = p2.getPos(i);
        if (!p1.hasNode(u) || !p2.hasNode(v)) {
            p1.replaceNode(i, u);
            p2.replaceNode(i, v);
        }
    }
}

std::vector<int> IDH(Population p1, InfluenceGraph g) {
    int influenceDistance[g.numVertex()];
    for (int i = 0;i < g.numVertex();++i) {
        influenceDistance[i] = g.numVertex();
        if (p1.hasNode(i)) {
            influenceDistance[i] = 0;
        } else {
            for (int j = 0;j < g.numVertex();++j) {
                if (p1.hasNode(j) && g.shortestPath(i,j) > influenceDistance[i]) {
                    influenceDistance[i] = g.shortestPath(i,j);
                }
            }
        }
    }
    int maxDistance = *std::max_element(&influenceDistance[0], &influenceDistance[0] + g.numVertex());
    int d = (maxDistance - 2)/2 + 2;
    std::vector<int> C;
    for (int i = 0;i < g.numVertex();++i) {
        if (influenceDistance[i] > d) {
            C.push_back(i);
        }
    }

    return C;
}

void mutation(Population &p, InfluenceGraph &g) {
    std::vector<double> scores(g.numVertex());
    std::vector<double> inverseScores(g.numVertex());

    for (int i = 0;i < g.numVertex();++i) {
        scores[i] = g.spread(i);
        inverseScores[i] = 1.0/double(g.spread(i));
    }

    std::vector<int> C = IDH(p, g);
    std::vector<int> nodes = p.getNodes();
    roulette_shuffle(C, scores);
    roulette_shuffle(nodes, inverseScores);
    p.setNodes(nodes);

    int lambda = rand() % (std::min(C.size(), p.size()) + 1);
    for (int i = 0;i < lambda;++i) {
        p.replaceNode(i, C[i]);
    }
}

Population genetic(InfluenceGraph g, int k, int stopConditionNumber) {
    srand(time(NULL));
    std::vector<Population> populations(M);
    for (int i = 0;i < M;++i) {
        populations[i] = Population::random(g.numVertex(),k);
    }
    int currentSolutionsExplored = 0;
    while(currentSolutionsExplored < stopConditionNumber && populations[0].getSpread() < g.numVertex()) {
        std::vector<std::vector<Population>> clonePopulations(N,populations);
        if (((float) rand() / (RAND_MAX)) < CP) {
            int rp1 = rand() % N, rp2 = rand() % (N-1), p1 = rand() % M, p2 = rand() % M;
            if (rp2 == rp1) ++rp2;
            crossover(clonePopulations[rp1][p1], clonePopulations[rp2][p2], g);
        }
        for (int i = 0;i < N;++i) {
            for (int j = 0;j < M;++j) {
                if (((float) rand() / (RAND_MAX)) < MP) {
                    mutation(clonePopulations[i][j], g);
                }
            }
        }
        std::vector<Population> bestPopulations(N);
        for (int i = 0;i < N;++i) {
            int bestScore = 0;
            for (int j = 0;j < M;++j) {
                clonePopulations[i][j].setSpread(g.spread(clonePopulations[i][j].getSeed()));

                if (clonePopulations[i][j].getSpread() > clonePopulations[i][bestScore].getSpread()) {
                    bestScore = j;
                }
            }
            bestPopulations[i] = clonePopulations[i][bestScore];
        }
        std::sort(bestPopulations.begin(), bestPopulations.end(), std::greater<Population>());
        std::copy(bestPopulations.begin(), bestPopulations.begin() + M, populations.begin());
        currentSolutionsExplored += N*M;
    }

    return populations[0];
}

/* ----- ACO ALGORITHM -----*/

/*
 * Adding Negative Learning to Ant Colony Optimization: A Comprehensive Study
 * https://doi.org/10.3390/math9040361
 */

Population genetateAcoSolution(InfluenceGraph g, int k, std::vector<double> scores, double deterministicRate) {
    Population population(g.numVertex());
    for (int i = 0;i < k && population.getSpread() < g.numVertex();++i) {
        std::vector<int> nodes(g.numVertex() - population.getSpread());
        double individualSpreadSum = 0.0;
        int pos = 0;
        for (int j = 0;j < g.numVertex();++j) {
            if (!population.isInfluenced(j)) {
                nodes[pos++] = j;
                individualSpreadSum += scores[j];
            }
        }

        double deterministicSol = std::rand()/double(RAND_MAX);
        int node;
        if (deterministicSol < deterministicRate) {
            node = roulette(nodes,0,scores,individualSpreadSum);
        } else {
            node = rand() % nodes.size();
        }

        std::vector<bool> actives = population.getInfluenced();
        int spread = g.addToCurrentSpread(actives, population.getSpread(), nodes[node]);
        
        population.addNode(nodes[node]);
        population.setInfluenced(actives);
        population.setSpread(spread);
    }

    return population;
}

Population aco(InfluenceGraph g, int k, int stopConditionNumber) {
    return aco(g,k,stopConditionNumber,DETERMINISTIC_RATE);
}

Population aco(InfluenceGraph g, int k, int stopConditionNumber, double deterministicRate) {
    std::vector<double> pheromone(g.numVertex(), 0.5);
    std::vector<double> scores(g.numVertex());
    for (int v = 0;v < g.numVertex();++v) {
        scores[v] = double(g.spread(v))*pheromone[v];
    }

    Population bestSol, restartSol;
    bool bs_update = false;
    double cf = 0.0;

    int currentSolutionsExplored = 0;
    while(currentSolutionsExplored < stopConditionNumber && bestSol.getSpread() < g.numVertex()) {
        std::vector<Population> iSols(N_ANTS);
        Population iSol[N_ANTS];
        for (int na = 0;na < N_ANTS;++na) {
            iSol[na] = genetateAcoSolution(g, k, scores, deterministicRate);
        }
        Population iBestSol = *std::max_element(iSol, iSol + N_ANTS);
        if (iBestSol > restartSol) {
            restartSol = iBestSol;
        }
        if (iBestSol > bestSol) {
            bestSol = iBestSol;
        }

        double i_weight, r_weight, g_weight;
        if (bs_update) {
            i_weight = 0.0;
            r_weight = 0.0;
            g_weight = 1.0;
        }
        else {
            if (cf < 0.4) {
                i_weight = 1.0;
                r_weight = 0.0;
                g_weight = 0.0;
            }
            else if (cf < 0.6) {
                i_weight = 2.0/3.0;
                r_weight = 1.0/3.0;
                g_weight = 0.0;
            }
            else if (cf < 0.8) {
                i_weight = 1.0/3.0;
                r_weight = 2.0/3.0;
                g_weight = 0.0;
            }
            else {
                i_weight = 0.0;
                r_weight = 1.0;
                g_weight = 0.0;
            }
        }

        cf = 0.0;
        for (int i = 0; i < g.numVertex(); ++i) {
            // Contribution calculation
            double contribution = 0.0;
            if (i_weight > 0.0 && iBestSol.hasNode(i)) { contribution += i_weight; }
            if (r_weight > 0.0 && restartSol.hasNode(i)) { contribution += r_weight; }
            if (g_weight > 0.0 && bestSol.hasNode(i)) { contribution += g_weight; }

            // Pheromone modification
            pheromone[i] += (LEARNING_RATE * (contribution - pheromone[i]));
            if (pheromone[i] > TAU_MAX) { pheromone[i] = TAU_MAX; }
            if (pheromone[i] < TAU_MIN) { pheromone[i] = TAU_MIN; }

            // Recalculate score
            scores[i] = double(g.spread(i))*pheromone[i];
            
            cf += std::max(TAU_MAX - pheromone[i], pheromone[i] - TAU_MIN);
        }

        // Calculation of convergence factor
        cf = cf / (double(pheromone.size()) * (TAU_MAX - TAU_MIN));
        cf = (cf - 0.5) * 2.0;

        if (cf > 0.9999) {
            if (bs_update) {
                pheromone = std::vector<double>(g.numVertex(), 0.5);
                for (int v = 0;v < g.numVertex();++v) {
                    scores[v] = double(g.spread(v))*pheromone[v];
                }
                restartSol = Population();
            }
            cf = 0;
            bs_update = !bs_update;
        }
        currentSolutionsExplored += N_ANTS;
    }

    return bestSol;
}