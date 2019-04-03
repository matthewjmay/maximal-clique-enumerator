#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
#include <fstream>
#include "graph.h"
#include "maximal_clique_enumerator.h"
#include "load_balancer.h"

#ifdef DEBUG
#define DEBUG_STDERR(x) (std::cerr << (x))
#define DEBUG_STDOUT(x) (std::cout << (x))
//... etc
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
#endif

const std::string DEFAULT_OUTPUT_FILE = "out.txt";

char *getCmdOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return nullptr;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Must provide an input file" << std::endl;
        exit(1);
    }

    std::string inputFileName = argv[1], outputFileName;

    if (cmdOptionExists(argv, argv + argc, "-o")) {
        outputFileName = getCmdOption(argv, argv + argc, "-o");
    } else {
        outputFileName = DEFAULT_OUTPUT_FILE;
    }
    std::ofstream out;
    out.open(outputFileName);
    if (!out) {
        std::cerr << "Could not open output file" << std::endl;
        exit(1);
    }
    std::mutex outputBarrier;

    size_t numThreads = std::thread::hardware_concurrency();
    char *numThreadsOpt = getCmdOption(argv, argv + argc, "-n");
    if (numThreadsOpt) {
        numThreads = atoi(numThreadsOpt);
    }

    std::cout << "Enumerating cliques from graph: " << inputFileName << " into file " << outputFileName << std::endl;

    Graph graph{inputFileName};
    std::cout << "Parsed graph: " << std::endl << graph << std::endl;
    std::vector<std::thread> threads;
    LoadBalancer balancer;

    for (size_t i = 1; i < numThreads; ++i) {
        threads.emplace_back(MaximalCliqueEnumerator(i, graph, balancer, out, numThreads, outputBarrier));
    }

    MaximalCliqueEnumerator(0, graph, balancer, out, numThreads, outputBarrier)();
    for (auto &t: threads) {
        t.join();
    }

    out.close();

    return 0;
}