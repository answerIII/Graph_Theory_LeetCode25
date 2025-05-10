#ifndef GRAPH_H
#define GRAPH_H
#include <stack>

#include "libs.h"
#include "classes/Node.h"

class Graph {
protected:
    std::unordered_map<int, std::vector<int>>& paths;
    std::unordered_map<int, Node>& nodes;
    int vertexCount= 0;
    int edgesCount = 0;
    void removeMarks() {
        for (std::pair<const int, Node> node : nodes) {
            node.second.marked = false;
        }
    }
    int dfs(Node* node, std::unordered_map<int, std::vector<int>>& paths) const {
        std::stack<Node*> stack;
        stack.push(node);
        int nodeCount = 0;
        while (!stack.empty()) {
            Node* currentNode = stack.top();
            ++nodeCount;
            currentNode->marked = true;
            for (int neighborhood : paths[currentNode->num]) {
                if (nodes[neighborhood].marked != true) {
                    stack.push(&nodes[neighborhood]);
                }
            }
        }
        return nodeCount;
    }
public:
    Graph (std::unordered_map<int, Node>& nodes,
        std::unordered_map<int,std::vector<int>>& list,
        int vertexCount, int edgesCount)
        : paths(list), nodes(nodes) {
        this->vertexCount = vertexCount;
        this->edgesCount = edgesCount;
    }
    int getVertexCount() const {
        return vertexCount;
    }
    int getEdgesCount() const {
        return edgesCount;
    }
};

class DirectedGraph : public Graph {
    std::unordered_map<int, std::vector<int>> transposePaths;
    std::unordered_map<int, std::vector<int>> undirectedPaths;

    double density = 0;
    int weekComponentCount = 0;
    int maxVertexCountInWeekComponent = 0;
    std::vector<Node*> biggestWeekNode;

    int strongComponentCount = 0;
    int maxVertexCountInStrongComponent = 0;
    std::vector<Node*>  biggestStrongNode;

    void findWeekComponentCount() {
        for (std::pair node : nodes) {
            if (!node.second.marked) {
                int nodeCount = dfs(&node.second, undirectedPaths);
                ++weekComponentCount;
                if (nodeCount > maxVertexCountInWeekComponent) {
                    maxVertexCountInWeekComponent = nodeCount;
                }
            }
        }
        removeMarks();
    }

    void findStrongestComponentCount() {
        // 1 dfs
        std::stack<Node*> outVertexes;
        for (auto& [num,node] : nodes) {
            if (!node.marked) {
                std::stack<Node*> attended;
                attended.push(&node);
                while (!attended.empty()) {
                    Node* current = attended.top();
                    current->marked = true;
                    bool allIsMarked = true;
                    for (int neighborhood: paths[current->num]) {
                        if (!nodes[neighborhood].marked) {
                            attended.push(&nodes[neighborhood]);
                            allIsMarked = false;
                        }
                    }
                    if (allIsMarked) {
                        outVertexes.push(current);
                        attended.pop();
                    }
                }
            }
        }
        removeMarks();
        // 2 reversed bfs using outVertexes stack
        initTransposePaths();
        for (Node* node = outVertexes.top(); !outVertexes.empty(); outVertexes.pop()) {
            if (!node->marked) {
                int nodeCount = dfs(node,transposePaths);
                ++strongComponentCount;
                if (nodeCount > maxVertexCountInStrongComponent) {
                    maxVertexCountInStrongComponent = nodeCount;
                }
            }
        }
        removeMarks();
    }

    void initUndirectedPaths() {
        undirectedPaths.reserve(paths.size());
        for (auto& [u, neigh] : paths) {
            for (int v : neigh) {
                undirectedPaths[u].push_back(v);
                undirectedPaths[v].push_back(u);
            }
        }
    }

    void initTransposePaths() {
        transposePaths.reserve(paths.size());
        for (auto& [u, neigh] : paths) {
            for (int v : neigh) {
                transposePaths[v].push_back(u);
            }
        }
    }

    void initDensity() {
        double maxEdges = vertexCount * (vertexCount - 1);
        density = edgesCount / maxEdges;
    }

public:

    int getWeekComponentCount() {
        if (strongComponentCount == 0 && weekComponentCount == 0) initUndirectedPaths();
        if (weekComponentCount == 0) findWeekComponentCount();
        return weekComponentCount;
    }

    int getStrongestComponentCount() {
        if (strongComponentCount == 0 && weekComponentCount == 0) initUndirectedPaths();
        if (strongComponentCount == 0) findStrongestComponentCount();
        return weekComponentCount;
    }

    double getDensity() {
        if (density == 0) initDensity();
        return density;
    }

    int getShareVertexInBeggestWeekComponent() {
        getWeekComponentCount();
        return maxVertexCountInWeekComponent / weekComponentCount;
    }

    int getShareVertexInBeggestStrongComponent() {
        getStrongestComponentCount();
        return maxVertexCountInWeekComponent / strongComponentCount;
    }

};

class UndirectedGraph : public Graph {
    double density = 0;
    int weekComponentCount = 0;
    int maxVertexCountInComponent = 0;
    Node* biggestNode = nullptr;

    void findWeekComponentCount() {
        for (std::pair node : nodes) {
            if (!node.second.marked) {
                int nodeCount = dfs(&node.second, paths);
                ++weekComponentCount;
                if (nodeCount > maxVertexCountInComponent) {
                    maxVertexCountInComponent = nodeCount;
                }
            }
        }
        removeMarks();
    }

    void initDensity() {
        double maxEdges = vertexCount * (vertexCount - 1) / 2.0;
        density = edgesCount / maxEdges;
    }

public:

    int getComponentCount() {
        if (weekComponentCount == 0) findWeekComponentCount();
        return weekComponentCount;
    }

    double getDensity() {
        if (density == 0) initDensity();
        return density;
    }

    int getShareVertexInBeggestComponent() {
        getComponentCount();
        return maxVertexCountInComponent / weekComponentCount;
    }

};


#endif //GRAPH_H
