#ifndef GRAPH_H
#define GRAPH_H
#include <queue>
#include <stack>

#include "../libs.h"
#include "Node.h"
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
    int approximateDiameter = 0;
    int percentileB = 0;
    int percentileC = 0;

    void initWeekComponents() {

        if (undirectedPaths.empty()) { initUndirectedPaths(); }

        for (auto& [num,node] : nodes) {
            if (!node.marked) {
                std::vector<Node*> component;
                std::queue<Node*> queue;
                queue.push(&node);
                node.marked = true;
                int nodeCount = 0;
                while (!queue.empty()) {
                    Node* currentNode = queue.front(); queue.pop();
                    component.push_back(currentNode);
                    ++nodeCount;
                    for (int neighborhood : undirectedPaths[currentNode->num]) {
                        if (nodes[neighborhood].marked != true) {
                            nodes[neighborhood].marked = true;
                            queue.push(&nodes[neighborhood]);
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
                std::queue <Node*> queue;
                queue.push(node);
                node->marked = true;
                while (!queue.empty()) {
                    Node* currentNode = queue.front(); queue.pop();
                    component.push_back(currentNode);
                    for (int neighborhood : transposePaths[currentNode->num]) {
                        if (nodes[neighborhood].marked != true) {
                            nodes[neighborhood].marked = true;
                            queue.push(&nodes[neighborhood]);
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

    void initApproximateDiameter() {

        if (weekComponents.empty()) initWeekComponents();

        int randomIndex = 1718 % weekComponents[0].size();
        Node* r  = weekComponents[0][randomIndex];
        std::pair<int, Node*> a = getFarthestVertexInsideWWC(r);
        std::pair<int, Node*> b = getFarthestVertexInsideWWC(a.second);
        approximateDiameter = b.first;
    }
    /// dangerous !!! might be weekComponents.size() * 1000 space !!!!! should be optimizated
    /// VERY VERY BAD
    /// DONT START IT !!!!
    /// YOUR COMPUTER GET DOWN
    void init90PercentileB() {

        if (weekComponents.empty()) initWeekComponents();

        //calculate samples count
        int samples = 500;
        int componentSize = weekComponents[0].size();
        if (componentSize < 500) samples = componentSize;
        std::vector<int> distances;
        distances.reserve(samples);

        //mark week component, work only with theLargestWCC nodes
        for (Node* node : weekComponents[0]) {
            node->marked = true;
        }

        //unsing a bfs try to find the distance
        //between two random nodes inside the LargestWCC
        //put result to distances
        for (int i =0; i < samples; ++i) {

            int index = rand() % componentSize;
            Node* u = weekComponents[0][index];
            index = rand() % componentSize;
            Node* v = weekComponents[0][index];

            std::queue<Node*> queue;
            std::unordered_map<int,int> lengths;
            lengths.reserve(samples);
            lengths[u->num] = 0;
            queue.push(u);
            while (!queue.empty()) {
                Node* currentNode = queue.front(); queue.pop();
                if (currentNode->num == v->num) break;
                for (int neighborhood : undirectedPaths[currentNode->num]) {
                    if (!nodes[neighborhood].marked || lengths.contains(neighborhood)) continue;
                    lengths[neighborhood] = lengths[currentNode->num] + 1;
                    queue.push(&nodes[neighborhood]);
                }
            }
            distances.push_back(lengths[v->num]);
        }

        sort(distances.begin(), distances.end());
        int index90 = (int)(0.9 * distances.size());
        percentileB = distances[index90];
    }
    //require optimization
    void init90PercentileC() {

        if (weekComponents.empty()) initWeekComponents();

        //create a snowball
        int snowballSize = vertexCount > 10000 ? 1000 : 500;
        int componentSize = weekComponents[0].size();
        if (componentSize < 500) snowballSize = componentSize;
        std::vector<int> snowball;
        snowball.reserve(snowballSize);

        //pull a snowball;
        Node* node = weekComponents[0][0];
        std::queue<Node*> queue;
        queue.push(node);
        node->marked = true;
        while (!queue.empty() && snowball.size() <= snowballSize) {
            Node* currentNode = queue.front(); queue.pop();
            snowball.push_back(currentNode->num);
            for (int neighborhood : undirectedPaths[currentNode->num]) {
                if (nodes[neighborhood].marked) continue;
                nodes[neighborhood].marked = true;
                queue.push(&nodes[neighborhood]);
            }
        }
        // DONT REMOVE marked because
        // we should build distances for each node in range of snowball graph
        // so i intend to consider only marked nodes below
        // not -> removeMarks();

        //just count all distances and calculate 90partentile
        std::vector<int> distances;
        distances.reserve(snowballSize);

        for (int index : snowball) {
            Node* node = &nodes[index];
            std::queue<Node*> queue;
            std::unordered_map<int,int> lengths;
            lengths[node->num] = 0;
            queue.push(node);
            while (!queue.empty()) {
                Node* currentNode = queue.front(); queue.pop();
                for (int neighborhood : undirectedPaths[currentNode->num]) {
                    // if not marked -> continue
                    if (!nodes[neighborhood].marked) continue;

                    //alternative way to mark
                    if (lengths.contains(neighborhood)){continue;}

                    lengths[neighborhood] = lengths[currentNode->num] + 1;
                    queue.push(&nodes[neighborhood]);
                }
            }
            for (auto& [num, len]:lengths) {
                distances.push_back(len);
            }
            removeMarks();
        }

        sort(distances.begin(), distances.end());
        int index90 = (int)(0.9 * distances.size());
        percentileC = distances[index90];
    }

    std::pair<int, Node*> getFarthestVertex(Node* node) {
        std::pair farthestVertex(0,node);
        std::queue<Node*> queue;
        std::unordered_map<int,int> lengths(nodes.size());
        lengths[node->num] = 0;
        queue.push(node);
        while (!queue.empty()) {
            Node* currentNode = queue.front(); queue.pop();
            currentNode->marked = true;
            if (farthestVertex.first < lengths[currentNode->num]) {
                farthestVertex.first = lengths[currentNode->num];
                farthestVertex.second = currentNode;
            }
            for (int neighborhood : undirectedPaths[currentNode->num]) {
                if (nodes[neighborhood].marked) continue;
                nodes[neighborhood].marked = true;
                lengths[neighborhood] = lengths[currentNode->num] + 1;
                queue.push(&nodes[neighborhood]);
            }
        }
        removeMarks();
        return farthestVertex;
    }

    std::pair<int, Node*> getFarthestVertexInsideWWC(Node* node) {
        removeMarks();
        //mark all nodes belong to largestWCC
        for (Node* wccNode : weekComponents[0]) {
            wccNode->marked = true;
        }
        //find the farthest from the node
        std::pair farthestVertex(0,node);
        std::queue<Node*> queue;
        std::unordered_map<int,int> lengths(nodes.size());
        std::unordered_set<int> markedV(nodes.size());
        lengths[node->num] = 0;
        queue.push(node);
        node->marked = true;
        markedV.insert(node->num);
        while (!queue.empty()) {
            Node* currentNode = queue.front(); queue.pop();
            if (farthestVertex.first < lengths[currentNode->num]) {
                farthestVertex.first = lengths[currentNode->num];
                farthestVertex.second = currentNode;
            }
            for (int neighborhood : undirectedPaths[currentNode->num]) {
                if (!nodes[neighborhood].marked || markedV.contains(nodes[neighborhood].num)) continue;
                markedV.insert(nodes[neighborhood].num);
                lengths[neighborhood] = lengths[currentNode->num] + 1;
                queue.push(&nodes[neighborhood]);
            }
        }
        removeMarks();
        return farthestVertex;
    }

public:

    size_t getWeekComponentCount() {
        if (weekComponents.empty()) initWeekComponents();
        return weekComponents.size();
    }

    size_t getStrongestComponentCount() {
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

    int getApproximateDiameter() {
        if (approximateDiameter == 0) initApproximateDiameter();
        return approximateDiameter;
    }

    int get90PercentileB() {
        if (percentileB == 0) init90PercentileB();
        return percentileB;
    }

    int get90PercentileC() {
        if (percentileC == 0) init90PercentileC();
        return percentileC;
    }

    size_t getCountNodesInLargestWCC() {
        if (weekComponents.empty()) initWeekComponents();
        return weekComponents[0].size();
    }

    size_t getCountNodesInLargestSCC() {
        if (weekComponents.empty()) initStrongComponents();
        return strongComponents[0].size();
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
