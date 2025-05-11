#include <iostream>

#include "classes/Graph.h"
#include "classes/Parser.h"
#include "functions/parserf.h"
int main(const int argc, const char *argv[]) {
    validateArgs(argc, argv);
    bool directed = (std::string(argv[2]) == std::string("directed"));
    TxtParser parser;
    Graph graph(parser.parse(argv[1], directed));
    DirectedGraph directedGraph(graph);
    std::cout << directedGraph.getWeekComponentCount() << std::endl;
    std::cout << directedGraph.getStrongestComponentCount() << std::endl;
    std::cout << directedGraph.getDensity() << std::endl;
    std::cout << directedGraph.getShareVertexInBeggestWeekComponent()<< std::endl;
    std::cout << directedGraph.getEdgesCount()<< std::endl;
    return 0;
}