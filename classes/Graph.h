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
    std::vector<std::vector<Node*>>  components;
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

    double density = 0;
    int weekComponentCount = 0;
    int maxVertexCountInWeekComponent = 0;
    //std::vector<Node*> biggestWeekNode;

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

    void initUndirectedPaths() {
        undirectedPaths.reserve(paths.size());
        for (auto& [u, neigh] : paths) {
            for (int v : neigh) {
                undirectedPaths[u].push_back(v);
                undirectedPaths[v].push_back(u);
            }
        }
    }

    int countStrongComponents() {
    // 1) Построить стек вершин в порядке окончания DFS (по post‑order), нерекурсивно
    std::stack<Node*> finishStack;

    // Сбросить метки
    for (auto & [k, nd] : nodes) nd.marked = false;

    // Для каждого узла, если не посещён — запустить DFS
    for (auto & [k, nd] : nodes) {
        if (nd.marked) continue;
        // используем стек пар (Node*, индекс следующего соседа)
        std::stack<std::pair<Node*, size_t>> st;
        nd.marked = true;
        st.emplace(&nd, 0);

        while (!st.empty()) {
            auto & [u, idx] = st.top();
            auto & neigh = paths[u->num];

            if (idx < neigh.size()) {
                int vnum = neigh[idx++];
                Node & v = nodes[vnum];
                if (!v.marked) {
                    v.marked = true;
                    st.emplace(&v, 0);
                }
            } else {
                // все потомки обработаны — фиксируем u в порядке окончания
                finishStack.push(u);
                st.pop();
            }
        }
    }

    // 2) Сбросить метки для второго прохода
    for (auto & [k, nd] : nodes) nd.marked = false;

    // 3) Построить транспонированный граф, если ещё не построен
    if (transposePaths.empty()) {
        for (auto & [u, neigh] : paths) {
            for (int v : neigh) {
                transposePaths[v].push_back(u);
            }
        }
    }

    // 4) Второй проход: обойти в порядке finishStack и посчитать компоненты
    int sccCount = 0;
    while (!finishStack.empty()) {
        Node * start = finishStack.top();
        finishStack.pop();
        if (start->marked) continue;

        // новый DFS по transposePaths
        ++sccCount;
        std::stack<Node*> st2;
        start->marked = true;
        st2.push(start);

        while (!st2.empty()) {
            Node * u = st2.top();
            st2.pop();
            for (int vnum : transposePaths[u->num]) {
                Node & v = nodes[vnum];
                if (!v.marked) {
                    v.marked = true;
                    st2.push(&v);
                }
            }
        }
    }

    return sccCount;
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

    void initComponents() {
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
                components.push_back(component);
            }
        }
        removeMarks();
        //sorting [0] - the biggest component
        std::sort(components.begin(), components.end(),
    [this](const std::vector<Node*>& a, const std::vector<Node*>& b) {
        return a.size() > b.size();}
        );
    }
public:

    int getWeekComponentCount() {
        if (components.empty() && weekComponentCount == 0) initUndirectedPaths();
        if (weekComponentCount == 0) findWeekComponentCount();
        return weekComponentCount;
    }

    int getStrongestComponentCount() {
        if (components.empty() == 0 && weekComponentCount == 0) initUndirectedPaths();
        if (components.empty()) initComponents();
        return components.size();
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
        return components[0].size() / vertexCount;
    }

    DirectedGraph(Graph& graph)
    : Graph(graph) {}

};

class UndirectedGraph : public Graph {
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
                components.push_back(component);
            }
        }
        removeMarks();
        std::ranges::sort(components);
    }

public:

    int getComponentCount() {
        if (components.empty()) initComponents();
        return components.size();
    }

    double getDensity() {
        if (density == 0) initDensity();
        return density;
    }

    int getShareVertexInBeggestComponent() {
        getComponentCount();
        return components[0].size() / vertexCount;
    }

};


#endif //GRAPH_H
