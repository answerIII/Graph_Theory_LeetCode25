#ifndef GRAPH_H
#define GRAPH_H
#include <queue>
#include <execution>
#include <stack>

#include "../libs.h"
#include "Node.h"

namespace stdr = std::ranges;

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
    int trianglesCount = 0;

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

    void init90PercentileB() {

        if (weekComponents.empty()) initWeekComponents();

        //calculate samples count
        int samples = 500;
        int componentSize = weekComponents[0].size();
        if (componentSize < 500) samples = componentSize;
        std::vector<int> distances;
        distances.reserve(samples);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, componentSize - 1);

        //mark week component, work only with theLargestWCC nodes
        for (Node* node : weekComponents[0]) {
            node->marked = true;
        }

        //make calculation parralel i have 12 logic threads
        int numThreads = std::min(samples, 12);
        std::mutex distMutex;

        //unsing a bfs try to find the distance
        //between two random nodes inside the LargestWCC
        //put result to distances
        auto worker = [&](Node* u, Node* v) {
            std::queue<Node*> queue;
            std::unordered_map<int, int> lengths;
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
            // thead safety add len
            std::lock_guard<std::mutex> lock(distMutex);
            if (lengths.contains(v->num)) {
                distances.push_back(lengths[v->num]);
            }
        };

        std::vector<std::thread> threads;
        int perThread = samples / numThreads;
        int remainder = samples % numThreads;
        int start = 0;

        //how many times the thread should compute  worker(u, v);
        auto workerBatch = [&](int count) {
            for (int i = 0; i < count; ++i) {
                Node* u = weekComponents[0][dis(gen)];
                Node* v = weekComponents[0][dis(gen)];
                worker(u, v);
            }
        };

        for (int i = 0; i < numThreads; ++i) {
            int count = start + perThread + (i < remainder ? 1 : 0);
            //emplace_back like push_back, but object creating inside vector
            threads.emplace_back(workerBatch, count);
        }

        for (auto& t : threads) t.join();

        if (distances.empty()) {
            percentileB = -1;
            return;
        }



        // //unsing a bfs try to find the distance
        // //between two random nodes inside the LargestWCC
        // //put result to distances
        // for (int i =0; i < samples; ++i) {
        //
        //     int index = rand() % componentSize;
        //     Node* u = weekComponents[0][index];
        //     index = rand() % componentSize;
        //     Node* v = weekComponents[0][index];
        //
        //     std::queue<Node*> queue;
        //     std::unordered_map<int,int> lengths;
        //     lengths.reserve(samples);
        //     lengths[u->num] = 0;
        //     queue.push(u);
        //     while (!queue.empty()) {
        //         Node* currentNode = queue.front(); queue.pop();
        //         if (currentNode->num == v->num) break;
        //         for (int neighborhood : undirectedPaths[currentNode->num]) {
        //             if (!nodes[neighborhood].marked || lengths.contains(neighborhood)) continue;
        //             lengths[neighborhood] = lengths[currentNode->num] + 1;
        //             queue.push(&nodes[neighborhood]);
        //         }
        //     }
        //     distances.push_back(lengths[v->num]);
        // }

        sort(distances.begin(), distances.end());
        int index90 = (int)(0.9 * distances.size());
        percentileB = distances[index90];
    }

    void init90PercentileC() {

        if (weekComponents.empty()) initWeekComponents();

        //create a snowball
        int snowballSize = 500;
        int componentSize = weekComponents[0].size();
        if (componentSize < 500) snowballSize = componentSize;
        std::vector<Node*> snowball;
        snowball.reserve(snowballSize);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, snowballSize - 1);
        int numThreads = std::min(snowballSize, 12);
        std::mutex distMutex;

        //pull a snowball;
        Node* node = weekComponents[0][0];
        std::queue<Node*> queue;
        queue.push(node);
        node->marked = true;
        while (!queue.empty() && snowball.size() <= snowballSize) {
            Node* currentNode = queue.front(); queue.pop();
            snowball.push_back(currentNode);
            for (int neighborhood : undirectedPaths[currentNode->num]) {
                if (nodes[neighborhood].marked) continue;
                nodes[neighborhood].marked = true;
                queue.push(&nodes[neighborhood]);
            }
        }

        std::vector<int> distances;
        distances.reserve(snowballSize);

        auto worker = [&](Node* u, Node* v) {
            std::queue<Node*> queue;
            std::unordered_map<int, int> lengths;
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
            // thead safety add len
            std::lock_guard<std::mutex> lock(distMutex);
            if (lengths.contains(v->num)) {
                distances.push_back(lengths[v->num]);
            }
        };

        //how many times the thread should compute  worker(u, v);
        auto workerBatch = [&](int count) {
            for (int i = 0; i < count; ++i) {
                Node* u = snowball[dis(gen)];
                Node* v = snowball[dis(gen)];
                worker(u, v);
            }
        };

        std::vector<std::thread> threads;
        int perThread = snowballSize / numThreads;
        int remainder = snowballSize % numThreads;
        int start = 0;

        for (int i = 0; i < numThreads; ++i) {
            int count = start + perThread + (i < remainder ? 1 : 0);
            //emplace_back like push_back, but object creating inside vector
            threads.emplace_back(workerBatch, count);
        }

        for (auto& t : threads) t.join();

        sort(distances.begin(), distances.end());
        int index90 = (int)(0.9 * distances.size());
        percentileC = distances[index90];
    }

    void initTringlesCount() {
        if (weekComponents.empty()) initWeekComponents();

        //make directed graph from undirected, direct from week(less degree) node to strong
        std::unordered_map<int, std::vector<int>> directedPaths;
        directedPaths.reserve(paths.size());
        for (auto& [num, vec] : paths) {
            for (int neighbour : vec) {
                // if (nodes[num].degree <= nodes[neighbour].degree) {
                //     directedPaths[num].push_back(neighbour);
                // }
            }
        }
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
    // int getTriangels() {

    // }

//   double getAverageClusteringCoefficient() {
//     double totalCoefficient = 0.0;
    
//     // Инициализация наибольшей компоненты слабой связности
//     initWeekComponents();

//     int countedVertices = weekComponents[0].size();

//     // Перебор всех вершин наибольшей компоненты слабой связности
//     // for (Node* node : weekComponents[0]) {
//     for (int x = 0; x < weekComponents[0].size(); x++) {
//         Node *node = weekComponents[0][x];

//         int u = node->num;
        
//         const std::vector<int>& neighbors = paths[u];

//         int k = neighbors.size();

//         int linkCount = 0;

//         // Подсчет числа рёбер между соседями
//         for (int i = 0; i < k; ++i) {
//             for (int j = i + 1; j < k; ++j) {
//                 int nj = neighbors[j];
//                 int ni = neighbors[i];

//                 // Проверка наличия обратной дуги (ориентированный граф)
//                 if (std::find(paths[ni].begin(), paths[ni].end(), nj) != paths[ni].end()) {
//                     linkCount++;
//                 }
//                 if (std::find(paths[nj].begin(), paths[nj].end(), ni) != paths[nj].end()) {
//                     linkCount++;
//                 }
//             }
//         }

//         if (k < 2) continue;

//         double Clu = (2.0 * linkCount) / (k * (k - 1));
//         totalCoefficient += Clu;
//     }

//     std::cout << "Total vertices in largest WCC: " << countedVertices << std::endl;
//     std::cout << "Total clustering sum: " << totalCoefficient << std::endl;

//     return countedVertices > 0 ? totalCoefficient / countedVertices : 0.0;
// }


// double getAverageClusteringCoefficient() {
//     double totalCoefficient = 0.0;

//     // Инициализация наибольшей компоненты слабой связности
//     initWeekComponents();

//     int countedVertices = weekComponents[0].size();

//     for (int x = 0; x < weekComponents[0].size(); x++) {
//         Node* node = weekComponents[0][x];
//         int u = node->num;

//         std::unordered_set<int> neighborSet;

//         // исходящие соседи
//         for (int v : paths[u]) {
//             neighborSet.insert(v);
//         }

//         // входящие соседи
//         for (int v = 0; v < vertexCount; ++v) {
//             for (int w : paths[v]) {
//                 if (w == u) {
//                     neighborSet.insert(v);
//                 }
//             }
//         }

//         std::vector<int> neighbors(neighborSet.begin(), neighborSet.end());
//         int k = neighbors.size();

//         if (k < 2) continue;

//         int linkCount = 0;

        // // Проверка наличия рёбер между всеми парами соседей
        // for (int i = 0; i < k; ++i) {
        //     int ni = neighbors[i];
        //     std::unordered_set<int> ni_neighbors(paths[ni].begin(), paths[ni].end());

        //     for (int j = i + 1; j < k; ++j) {
        //         int nj = neighbors[j];

        //         // Если есть хотя бы одно ребро между ni и nj
        //         if (ni_neighbors.count(nj) || 
        //             std::find(paths[nj].begin(), paths[nj].end(), ni) != paths[nj].end()) {
        //             linkCount++;
        //         }
        //     }
        // }

//         double Clu = (2.0 * linkCount) / (k * (k - 1));
//         totalCoefficient += Clu;
//     }

//     std::cout << "Total vertices in largest WCC: " << countedVertices << std::endl;
//     std::cout << "Total clustering sum: " << totalCoefficient << std::endl;

//     return countedVertices > 0 ? totalCoefficient / countedVertices : 0.0;
// }

double getAverageClusteringCoefficient() {
    std::atomic<double> totalCoefficient = 0.0;

    // Инициализация наибольшей компоненты слабой связности
    initWeekComponents();

    int countedVertices = weekComponents[0].size();

    // for (int x = 0; x < weekComponents[0].size(); x++) {
    //     Node* node = weekComponents[0][x];
    // for (Node *node : weekComponents[0]) {}
    // std::for_each(weekComponents[0].begin(), weekComponents[0].end(), [&](Node *node) {});
    std::for_each(std::execution::par, weekComponents[0].begin(), weekComponents[0].end(), [&](Node *node) {
    
        int u = node->num;

        // Собираем всех соседей (входящих и исходящих)
        std::unordered_set<int> neighborSet;

        // исходящие
        for (int v : paths[u]) {
            neighborSet.insert(v);
        }

        // входящие
        for (int v = 0; v < vertexCount; ++v) {
            for (int w : paths[v]) {
                if (w == u) {
                    neighborSet.insert(v);
                }
            }
        }

        std::vector<int> neighbors(neighborSet.begin(), neighborSet.end());
        int k = neighbors.size();

        if (k < 2) return;

        int linkCount = 0;

        // Считаем число связей между соседями (без учёта направления)
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                int ni = neighbors[i];
                int nj = neighbors[j];

                // Проверяем наличие ребра между соседями в любом направлении
                linkCount += (stdr::contains(paths[ni], nj)) || (stdr::contains(paths[nj], ni));
                
            }
        }

        // Кластерный коэффициент вершины
        double Clu = (2.0 * linkCount) / (k * (k - 1));
        totalCoefficient += Clu;
    });

    std::cout << "Total vertices in largest WCC: " << countedVertices << std::endl;
    std::cout << "Total clustering sum: " << totalCoefficient << std::endl;

    return countedVertices > 0 ? totalCoefficient / countedVertices : 0.0;
}
};

class UndirectedGraph : public Graph {
    std::vector<std::vector<Node*>>  strongComponents;
    std::unordered_map<int, std::vector<int>> undirectedPaths;
    std::vector<std::vector<Node*>>  weekComponents;

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
        std::ranges::sort(strongComponents, std::greater<>{});
    }

public:
    UndirectedGraph(Graph& graph): Graph(graph) {}

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

// double getAverageClusteringCoefficient() {
//     if (weekComponents.empty()) initWeekComponents(); // находим слабые компоненты

//     const std::vector<Node*>& largestWCC = weekComponents[0];

//     if (largestWCC.empty()) return 0.0;

//     double totalClustering = 0.0;

//     // Проходим по каждому узлу в компоненте
//     for (Node* node : largestWCC) {
//         int u = node->num;
//         const std::vector<int>& neighbors = undirectedPaths[u];

//         if (neighbors.size() < 2) {
//             // Кластерный коэффициент равен 0, если у узла менее двух соседей
//             continue;
//         }

//         int links = 0; // количество связей между соседями узла
//         // Проверяем, есть ли ребро между каждой парой соседей
//         for (size_t i = 0; i < neighbors.size(); ++i) {
//             int vi = neighbors[i];
//             for (size_t j = i + 1; j < neighbors.size(); ++j) {
//                 int vj = neighbors[j];

//                 // Проверяем, соединены ли vi и vj
//                 const auto& vi_neighbors = undirectedPaths[vi];
//                 if (std::find(vi_neighbors.begin(), vi_neighbors.end(), vj) != vi_neighbors.end()) {
//                     ++links;
//                 }
//             }
//         }

//         // Возможное количество связей между соседями: C(k,2) = k*(k-1)/2
//         int k = neighbors.size();
//         double clusteringCoefficient = (2.0 * links) / (k * (k - 1));
//         totalClustering += clusteringCoefficient;
//     }

//     return totalClustering / largestWCC.size();
// }

    void initUndirectedPaths() {
        undirectedPaths.reserve(paths.size());
        for (auto& [u, neigh] : paths) {
            for (int v : neigh) {
                undirectedPaths[u].push_back(v);
                undirectedPaths[v].push_back(u);
            }
        }
    }


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


 double getAverageClusteringCoefficient() {
    std::atomic<double> totalCoefficient = 0.0;

    // Инициализация наибольшей компоненты связности (для неориентированного графа — просто компонент связности)
    initWeekComponents();

    int countedVertices = weekComponents[0].size();

    std::for_each(std::execution::par, weekComponents[0].begin(), weekComponents[0].end(), [&](Node* node) {
        int u = node->num;

        // Все соседи вершины u
        const std::vector<int>& neighbors = paths[u];
        int k = neighbors.size();

        int linkCount = 0;

        // Проверяем количество связей между соседями
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                int ni = neighbors[i];
                int nj = neighbors[j];

                // Проверяем, связаны ли ni и nj
                linkCount += (stdr::contains(paths[ni], nj)) || (stdr::contains(paths[nj], ni));

            }
        }
        if (k < 2) return;
        // Кластерный коэффициент вершины
        double Clu = (2.0 * linkCount) / (k * (k - 1));
        totalCoefficient += Clu;
    });

    std::cout << "Total vertices in largest CC: " << countedVertices << std::endl;
    std::cout << "Total clustering sum: " << totalCoefficient << std::endl;
    std::cout<< "Average clustering coefficient: " << (countedVertices > 0 ? totalCoefficient / countedVertices : 0.0 ) << std::endl;
    return countedVertices > 0 ? totalCoefficient / countedVertices : 0.0;
}

};


#endif //GRAPH_H
