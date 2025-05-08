#ifndef GRAPH_H
#define GRAPH_H
#include <stack>

#include "../libs.h"
#include "Node.h"
class Graph {
    std::unordered_map<int, std::vector<int>>& paths;
    std::unordered_map<int, Node>& nodes;
    int vertexCount= 0;
    int edgesCount = 0;
    bool directed;

public:

    Graph (std::unordered_map<int, Node>& nodes,
        std::unordered_map<int,std::vector<int>>& list,
        int vertexCount, int edgesCount, bool directed)
        : paths(list), nodes(nodes) {
        this->vertexCount = vertexCount;
        this->edgesCount = edgesCount;
        this->directed = directed;
    }

};


#endif //GRAPH_H
