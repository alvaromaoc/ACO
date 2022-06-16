#include "InputOutputUtils.h"

AppConfig setAppConfig(int argc, char* argv[]) {
    AppConfig config;
    if (argc < 2) {
        config.error = "Error: Not enough arguments";
        return config;
    }
    for (int i = 1;i < argc;++i) {
        std::string flag = argv[i];
        if (flag.compare("-i") == 0) {
            if (argc <= i + 1) {
                config.error = "Error: Missing input newtowk path";
                return config;
            }
            config.networkPath = argv[++i];
        } else if (flag.compare("-g") == 0) {
            if (argc <= i + 1) {
                config.error = "Error: Missing graph iteration number";
                return config;
            }
            config.graphIterationNumber = stoi((std::string) argv[++i]);
        } else if (flag.compare("-x") == 0) {
            if (argc <= i + 1) {
                config.error = "Error: Missing stop condition";
                return config;
            }
            config.stopConditionNumber = stoi((std::string) argv[++i]);
        } else if (flag.compare("-r") == 0) {
            if (argc <= i + 2) {
                config.error = "Error: Missing rang";
                return config;
            }
            config.from = stoi((std::string) argv[++i]);
            config.to = stoi((std::string) argv[++i]);
        }
        else if (flag.compare("-p") == 0) { config.priorKnowledge = true; }
        else if (flag.compare("-s") == 0) { config.printSeed = true; }
        else if (flag.compare("-v") == 0) { config.printVariance = true; }
        else if (flag.compare("-c") == 0) { config.checkSeed = true; }
        else if (flag.compare("-e") == 0) { config.exportNetwork = true; }
        else if (flag.compare("-q") == 0) { config.stopOnReach = true; }
        else if (flag.compare("-a") == 0) {
            if (argc <= ++i) {
                config.error = "Error: Not enough algorithms";
                return config;
            }
            while (i < argc) {
                std::string algorithm = argv[i];
                if (algorithm[0] == '-') {
                    --i;
                    break;
                }

                // Finding runs
                int index = algorithm.find(":");
                std::string a = algorithm.substr(0,index);
                std::string r = "";
                if (index != -1) {
                    r = algorithm.substr(index+1, algorithm.length() - index - 1);
                }

                // Finding algorithm
                if (a.compare("greedy") == 0) { config.algorithms.push_back(Algorithm(config.folder,&greedy,a,r)); }
                else if (a.compare("celf") == 0) { config.algorithms.push_back(Algorithm(config.folder,&celf,a,r)); }
                else if (a.compare("genetic") == 0) { config.algorithms.push_back(Algorithm(config.folder,&genetic,a,r)); }
                else if (a.compare("aco") == 0) { config.algorithms.push_back(Algorithm(config.folder,&aco,a,r)); }
                else {
                    config.error = "Error: Algorithm \"" + algorithm + "\" not found.";
                    return config;
                }
                ++i;
            }
        } else {
            config.error = "Error: Unknown input parameter \"" + flag + "\".";
            return config;
        }
    }
    if (config.algorithms.size() == 0) {
        config.error = "Error: Not enough algorithms";
        return config;
    }
    if (config.networkPath.empty()) {
        config.error = "Error: Unknown network";
        return config;
    }
    return config;
}

std::vector<std::string> getWords(std::string str) {
    std::vector<std::string> vector;
    std::string word;
    std::stringstream iss(str);
 
    while (iss >> word) {
        vector.push_back(word);
    }
    return vector;
}

InfluenceGraph readSingleNetwork(std::fstream &file) {
    std::string tp;
    std::getline(file, tp);
    std::vector<std::string> words = getWords(tp);
    int vertices = std::stoi(words[2]), edges = stoi(words[3]);

    InfluenceGraph g(vertices, time(NULL));

    while(getline(file, tp)){ //read data from file object and put it into string.
        std::vector<std::string> words = getWords(tp);
        if (words[0].compare("e") == 0) {
            g.setEdge(std::stoi(words[1])-1, std::stoi(words[2])-1);
        } else {
            g.setThreshold(std::stoi(words[1])-1, std::stod(words[2]));
        }
    }
    file.close(); //close the file object.
    file.clear();
    return g;
}

std::vector<InfluenceGraph> readNetwork(std::string filePath, int graphIterationNumber) {
    const fs::path path(filePath);

    std::vector<InfluenceGraph> graphs;
    if (fs::is_directory(path)) {
        int g = 1;
        std::fstream file;
        file.open(filePath + "/network_g" + std::to_string(g) + ".dimacs");
        while (file.is_open()) {
            graphs.push_back(readSingleNetwork(file));
            ++g;
            file.open(filePath + "/network_g" + std::to_string(g) + ".dimacs");
        }
    } else {
        std::fstream newfile;
        newfile.open(filePath, std::ios::in); //open a file to perform read operation using file object
        if (!newfile.is_open()){ //checking whether the file is open
            throw "Error opening file";
        }
        std::string tp;
        getline(newfile, tp);
        std::vector<std::string> words = getWords(tp);
        int vertices = std::stoi(words[2]), edges = std::stoi(words[3]);

        for (int i = 0;i < graphIterationNumber;++i) {
            graphs.push_back(InfluenceGraph(vertices, time(NULL)+i));
        }

        while(getline(newfile, tp)){ //read data from file object and put it into string.
            std::vector<std::string> words = getWords(tp);
            if (words[0].compare("e") == 0) {
                for (int i = 0;i < graphs.size();++i) {
                    graphs[i].setEdge(std::stoi(words[1])-1, std::stoi(words[2])-1);
                }
            } else if (words[0].compare("t") == 0) {
                for (int i = 0;i < graphs.size();++i) {
                    graphs[i].setThreshold(std::stod(words[1])-1, std::stoi(words[2]));
                }
            }
        }
        newfile.close(); //close the file object.
    }
    return graphs;
    
}

void writeNetwork(std::vector<InfluenceGraph> graphs, std::string folder) {
    fs::create_directories("./Data/" + folder + "/networks");
    int i = 0;
    for (auto g = graphs.begin();g != graphs.end();++g) {
        std::ofstream file;
        if (THRESHOLD_TYPE == THRESHOLD_HALF) {
            file = std::ofstream(
                "./Data/" + folder + "/networks" +
                "/network_" + std::to_string(i++) + "_fix_" + std::to_string(g->getThreshold(0)).substr(0,4) + ".dimacs"
            );
        } else {
            file = std::ofstream(
                "./Data/" + folder + "/networks" +
                "/network_" + std::to_string(i++) + "_rand" + ".dimacs"
            );
        }
        std::string edges = "";
        std::string thresholds = "";
        int numEdges = 0;
        for (int r = 0;r < g->numVertex();++r) {
            thresholds += "t " + std::to_string(r+1) + " " + std::to_string(g->getThreshold(r)) + "\n";
            for (int c = r;c < g->numVertex();++c) {
                if (g->isNeighbour(r,c)) {
                    edges += "e " + std::to_string(r+1) + " " + std::to_string(c+1) + "\n";
                    ++numEdges;
                }
            }
        }
        file << "p edge " << g->numVertex() << " " << numEdges << std::endl << edges << thresholds;
    }
}

void printInfo(AppConfig &appConfig, int timePriorKnowledge) {
    std::ofstream offile("Data/" + appConfig.folder + "/info.yaml");
    if (!offile.is_open()) {
        throw "Error reading network!";
    }

    std::string info = "";

    // Network
    std::string network = appConfig.networkPath;
    int index = network.find("/");
    if (index != -1) network = network.substr(index+1,network.size());
    index = network.find(".");
    if (index != -1) network = network.substr(0,index);
    info += "Network: " + network + '\n';

    // Algorithms
    info += "Algorithms:\n";
    for (int i = 0;i < appConfig.algorithms.size();++i) {
        info += "\t" + appConfig.algorithms[i].name + ": " + std::to_string(appConfig.algorithms[i].runs) + "\n";
    }

    // Threshold
    info += "Threshold: ";
    if (THRESHOLD_TYPE == THRESHOLD_HALF) info += "0.5";
    if (THRESHOLD_TYPE == THRESHOLD_UNIFORM) info += "uniform";
    if (THRESHOLD_TYPE == THRESHOLD_RAND) info += "random";
    info += "\n";

    // Stop condition number
    info += "Stop condition number: " + std::to_string(appConfig.stopConditionNumber) + "\n";

    // Stop condition number
    info += "Graph iteration number: " + std::to_string(appConfig.graphIterationNumber) + "\n";

    // Stop condition number
    info += "Prior knowledge elapsed time (average): " + std::to_string(timePriorKnowledge);

    offile << info;
}