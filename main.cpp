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

    std::cout << "before : "<< directedGraph.getShareVertexInBeggestWeekComponent() << std::endl;
    std::cout << "after : "<< directedGraph.get90PercentileB() << std::endl;
    directedGraph.removeMostDegreesNodes(100000);
    std::cout << "after : "<< directedGraph.get90PercentileB() << std::endl;
    delete graph;
    return 0;
}
