#include <iostream>

#include "classes/Graph.h"
#include "classes/Parser.h"
#include "functions/parserf.h"
int main(const int argc, const char *argv[]) {
    validateArgs(argc, argv);
    bool directed = (std::string(argv[2]) == std::string("directed"));
    TxtParser parser;
    Graph* graph(parser.parse(argv[1], directed));

    DirectedGraph directedGraph(*graph);
    std::cout << directedGraph.get90PercentileC() << std::endl;


    delete graph;
    return 0;
}