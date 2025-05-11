#ifndef GRAPH_H
#define GRAPH_H
#include <stack>

#include "../libs.h"
#include "Node.h"
using namespace std;
class Graph {
protected:
    std::unordered_map<int, std::vector<int>> paths;
    std::unordered_map<int, Node> nodes;
    int vertexCount = 0;
    int edgesCount = 0;
    void removeMarks() {
        for (auto&[num,node] : nodes) {
            node.marked = false;
        }
    }
    int dfs(Node* node, std::unordered_map<int, std::vector<int>>& paths) {
        std::stack<Node*> stack;
        stack.push(node);
        int nodeCount = 0;
        while (!stack.empty()) {
            Node* currentNode = stack.top(); stack.pop();
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
    Graph (const std::unordered_map<int, Node> nodes,
        const std::unordered_map<int,std::vector<int>> paths,
        int vertexCount, int edgesCount) {
        this->paths = paths;
        this->nodes = nodes;
        this->vertexCount = vertexCount;
        this->edgesCount = edgesCount;
        std::cout << "copied graph" << std::endl;
    }

    Graph (Graph& graph) : paths(graph.paths), nodes(graph.nodes), vertexCount(graph.vertexCount), edgesCount(graph.edgesCount) {
        std::cout << "Graph created" << std::endl;
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
    std::vector<std::vector<Node*>>  strongComponents;
    std::vector<std::vector<Node*>>  weekComponents;

    double density = 0;
    //std::vector<Node*> biggestWeekNode;

    void initWeekComponents() {
        for (auto& [num,node] : nodes) {
            if (!node.marked) {
                std::vector<Node*> component;
                std::stack<Node*> stack;
                stack.push(&node);
                int nodeCount = 0;
                while (!stack.empty()) {
                    Node* currentNode = stack.top(); stack.pop();
                    component.push_back(currentNode);
                    ++nodeCount;
                    currentNode->marked = true;
                    for (int neighborhood : paths[currentNode->num]) {
                        if (nodes[neighborhood].marked != true) {
                            stack.push(&nodes[neighborhood]);
                        }
                    }
                }
                weekComponents.push_back(component);
            }
        }
        std::sort(weekComponents.begin(), weekComponents.end(),
    [this](const std::vector<Node*>& a, const std::vector<Node*>& b) {
        return a.size() > b.size();}
        );
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
        double maxEdges = vertexCount * (vertexCount - 1); // overflow!!!
        density = edgesCount / maxEdges;
    }

    void initStrongComponents() {
        //1 dfs
        std::vector<Node*> outVertexes;
        std::unordered_set<int> visited;
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
                        if (!visited.contains(current->num)) {
                            outVertexes.push_back(current);
                            visited.insert(current->num);
                        }
                        attended.pop();
                    }
                }
            }
        }
        removeMarks();
        // 2 transpose matrix
        initTransposePaths();
        // 3 reversed bfs using outVertexes stack - > find strong components
        for (auto it = outVertexes.rbegin(); it != outVertexes.rend(); ++it) {
            Node* node = *it;
            if (!node->marked) {
                std::vector <Node*> component;
                std::stack <Node*> stack;
                stack.push(node);
                while (!stack.empty()) {
                    Node* currentNode = stack.top(); stack.pop();
                    component.push_back(currentNode);
                    currentNode->marked = true;
                    for (int neighborhood : transposePaths[currentNode->num]) {
                        if (nodes[neighborhood].marked != true) {
                            stack.push(&nodes[neighborhood]);
                        }
                    }
                }
                strongComponents.push_back(component);
            }
        }
        removeMarks();
        //sorting [0] - the biggest component
        std::sort(strongComponents.begin(), strongComponents.end(),
    [this](const std::vector<Node*>& a, const std::vector<Node*>& b) {
        return a.size() > b.size();}
        );
    }
public:

    int getWeekComponentCount() {
        if (strongComponents.empty() && weekComponents.empty()) initUndirectedPaths();
        if (weekComponents.empty()) initWeekComponents();
        return weekComponents.size();
    }

    int getStrongestComponentCount() {
        if (strongComponents.empty() == 0 && weekComponents.empty()) initUndirectedPaths();
        if (strongComponents.empty()) initStrongComponents();
        return strongComponents.size();
    }

    double getDensity() {
        if (density == 0) initDensity();
        return density;
    }

    double getShareVertexInBeggestWeekComponent() {
        getWeekComponentCount();
        return (double)weekComponents[0].size() / (double)vertexCount * 100.0;
    }

    double getShareVertexInBeggestStrongComponent() {
        getStrongestComponentCount();
        return (double)strongComponents[0].size() / (double)vertexCount * 100.0;
    }

    DirectedGraph(Graph& graph)
    : Graph(graph) {}

};

class UndirectedGraph : public Graph {
    std::vector<std::vector<Node*>>  strongComponents;
    double density = 0;

    void initDensity() {
        double maxEdges = vertexCount * (vertexCount - 1) / 2.0;
        density = edgesCount / maxEdges;
    }

    void initComponents() {
        for (auto& [num,node] : nodes) {
            if (!node.marked) {
                std::vector <Node*> component;
                std::stack <Node*> stack;
                stack.push(&node);
                while (!stack.empty()) {
                    Node* currentNode = stack.top(); stack.pop();
                    component.push_back(currentNode);
                    currentNode->marked = true;
                    for (int neighborhood : paths[currentNode->num]) {
                        if (nodes[neighborhood].marked != true) {
                            stack.push(&nodes[neighborhood]);
                        }
                    }
                }
                strongComponents.push_back(component);
            }
        }
        removeMarks();
        std::ranges::sort(strongComponents);
    }

public:

    int getComponentsCount() {
        if (strongComponents.empty()) initComponents();
        return strongComponents.size();
    }

    double getDensity() {
        if (density == 0) initDensity();
        return density;
    }

    int getShareVertexInBeggestComponent() {
        getComponentsCount();
        return strongComponents[0].size() / vertexCount;
    }

};


#endif //GRAPH_H
