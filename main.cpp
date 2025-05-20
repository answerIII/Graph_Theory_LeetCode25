#include <iostream>
#include "classes/Graph.h"
#include "classes/Parser.h"
#include "functions/parserf.h"
#include "functions/CsvParser.h"
int main(const int argc, const char *argv[]) {
    validateArgs(argc, argv);
    bool directed = (std::string(argv[2]) == "directed");

    TxtParser parser;
    Graph* graph(parser.parse(argv[1], directed));

    DirectedGraph directedGraph(*graph);
    std::cout << directedGraph.get90PercentileC() << std::endl;

    if (directed) {
        DirectedGraph directedGraph(*graph);
       // std::cout << "90 percentile B: " << directedGraph.get90PercentileB() << std::endl;
        std::cout << "Average clustering coefficient: "<< directedGraph.getAverageClusteringCoefficient() << std::endl;
    } else {
        UndirectedGraph undirectedGraph(*graph);
        std::cout << undirectedGraph.getAverageClusteringCoefficient();
    }
    //degree_distribution(argv[1], argv[2], directedGraph);
    delete graph;
    return 0;
}
