#include "Graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <cstdio>
#include <thread>
#include <mutex>
#include <vector>
#include <string_view>
#include <algorithm>
#include <atomic>
#include <stack>
#include <functional>
#include <random>
#include <numeric>

Graph::Graph() : numVertices(0), numEdges(0), isLargeGraph(false) {}  

void Graph::loadFromFile(const std::string& path, const std::string& format) {
    edges.clear();
    reverseEdges.clear();
    numEdges = 0;
    numVertices = 0;
    isLargeGraph = false;

    std::unordered_set<int> vertexSet;
    std::mutex vertexMutex;

    FILE* file = fopen(path.c_str(), "rb");
    if (!file) throw std::runtime_error("Cannot open file: " + path);

    constexpr size_t BUFFER_SIZE = 128 * 1024 * 1024; // 128MB
    std::vector<char> buffer(BUFFER_SIZE);
    size_t bytesRead = fread(buffer.data(), 1, BUFFER_SIZE, file);
    fclose(file);
    if (bytesRead == 0) return;

    int skipFirst = (format == "csv") ? 1 : (format == "mtx") ? 2 : 0;
    char commentPrefix = (format == "txt") ? '#' : '%';

    std::vector<std::string_view> lines;
    size_t start = 0;
    for (size_t i = 0; i < bytesRead; ++i) {
        if (buffer[i] == '\n' || buffer[i] == '\r') {
            if (i > start) lines.emplace_back(&buffer[start], i - start);
            start = i + 1;
        }
    }
    if (start < bytesRead) lines.emplace_back(&buffer[start], bytesRead - start);

    if (skipFirst > 0 && lines.size() > static_cast<size_t>(skipFirst)) {
        lines.erase(lines.begin(), lines.begin() + skipFirst);
    }

    std::mutex edgeMutex;
    std::atomic<int> globalEdgeCount{0};

    size_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;

    std::vector<std::thread> threads;
    std::vector<std::unordered_map<int, std::vector<int>>> localEdges(numThreads);
    std::vector<std::unordered_map<int, std::vector<int>>> localReverseEdges(numThreads);

    auto worker = [&](size_t threadId, size_t begin, size_t end) {
        auto& localE = localEdges[threadId];
        auto& localR = localReverseEdges[threadId];
        int localCount = 0;

        for (size_t i = begin; i < end; ++i) {
            std::string_view line = lines[i];
            if (line.empty() || line[0] == commentPrefix) continue;

            const char* ptr = line.data();
            char* endPtr;
            int u = std::strtol(ptr, &endPtr, 10);
            if (endPtr == ptr) continue;

            while (*endPtr == ' ' || *endPtr == ',' || *endPtr == '\t') ++endPtr;
            char* endPtr2;
            int v = std::strtol(endPtr, &endPtr2, 10);
            if (endPtr == endPtr2) continue;
            if (u < 0 || v < 0) continue;

            {
                std::lock_guard<std::mutex> lock(vertexMutex);
                vertexSet.insert(u);
                vertexSet.insert(v);
            }

            localE[u].push_back(v);
            localR[v].push_back(u);
            localCount++;
        }

        {
            std::lock_guard<std::mutex> lock(edgeMutex);
            for (const auto& [u, vec] : localE)
                edges[u].insert(edges[u].end(), vec.begin(), vec.end());
            for (const auto& [v, vec] : localR)
                reverseEdges[v].insert(reverseEdges[v].end(), vec.begin(), vec.end());
        }
        globalEdgeCount.fetch_add(localCount, std::memory_order_relaxed);
    };

    size_t chunk = lines.size() / numThreads;
    for (size_t t = 0; t < numThreads; ++t) {
        size_t b = t * chunk;
        size_t e = (t + 1 == numThreads) ? lines.size() : (t + 1) * chunk;
        threads.emplace_back(worker, t, b, e);
    }
    for (auto& th : threads) th.join();

    numEdges = globalEdgeCount.load();
    numVertices = static_cast<int>(vertexSet.size());
    isLargeGraph = (numVertices > 100000 || numEdges > 1000000);
    isDirected = (path.find("directed") != std::string::npos && path.find("undirected") == std::string::npos);
}


int Graph::getVertexCount() const {
    return numVertices;
}

int Graph::getEdgeCount() const {
    return numEdges;
}

int Graph::bfsComponent(int start, std::unordered_set<int>& visited) {
    std::queue<int> q;
    q.push(start);
    visited.insert(start);
    int size = 0;

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        std::vector<int> neighbors;
        if (edges.count(v)) neighbors.insert(neighbors.end(), edges.at(v).begin(), edges.at(v).end());
        if (reverseEdges.count(v)) neighbors.insert(neighbors.end(), reverseEdges.at(v).begin(), reverseEdges.at(v).end());

        for (int u : neighbors) {
            if (visited.find(u) == visited.end()) {
                visited.insert(u);
                q.push(u);
                size++;
            }
        }
    }
    return size;
}

int Graph::countWeaklyConnectedComponentsBFS() {
    std::unordered_set<int> visited;
    int components = 0;
    int maxSize = 0;
    int size;

    for (int v = 0; v < numVertices; ++v) {
        if (visited.find(v) == visited.end()) {
            size = bfsComponent(v, visited);
            components++;
        }
        maxSize = std::max(maxSize, size);
    }
    wccRatio = (numVertices > 0) ? static_cast<double>(maxSize) / numVertices : 0.0;
    return components;
}

void Graph::dsuInit(int n) const {
    parent.resize(n);
    rank.resize(n, 0);
    for (int i = 0; i < n; ++i)
        parent[i] = i;
}

int Graph::dsuFind(int x) const {
    if (parent[x] != x)
        parent[x] = dsuFind(parent[x]);
    return parent[x];
}

void Graph::dsuUnion(int x, int y) const {
    int rootX = dsuFind(x);
    int rootY = dsuFind(y);
    if (rootX == rootY) return;

    if (rank[rootX] < rank[rootY]) {
        parent[rootX] = rootY;
    } else if (rank[rootX] > rank[rootY]) {
        parent[rootY] = rootX;
    } else {
        parent[rootY] = rootX;
        rank[rootX]++;
    }
}

int Graph::countWeaklyConnectedComponentsDSU() {
    dsuInit(numVertices);

    for (const auto& [u, neighbors] : edges) {
        for (int v : neighbors) {
            dsuUnion(u, v);
        }
    }

    for (const auto& [v, rev] : reverseEdges) {
        for (int u : rev) {
            dsuUnion(u, v); 
        }
    }

    std::unordered_map<int, int> componentSizes;

    for (int i = 0; i < numVertices; ++i) {
        if (edges.count(i) || reverseEdges.count(i)) {
            int root = dsuFind(i);
            componentSizes[root]++;
        }
    }

    int maxSize = 0;
    for (const auto& [_, size] : componentSizes)
        maxSize = std::max(maxSize, size);

    wccRatio = (numVertices > 0) ? static_cast<double>(maxSize) / numVertices : 0.0;
    return static_cast<int>(componentSizes.size());
}

int Graph::countWeaklyConnectedComponents() {
    return isLargeGraph ? countWeaklyConnectedComponentsDSU() : countWeaklyConnectedComponentsBFS();
}

double Graph::getDensity() const {
    if (numVertices <= 1) return 0.0;

    double maxEdges = isDirected
        ? static_cast<double>(numVertices) * (numVertices - 1)
        : static_cast<double>(numVertices) * (numVertices - 1) / 2;

    return static_cast<double>(numEdges) / maxEdges;
}

int Graph::countStronglyConnectedComponents() {
    std::vector<bool> visited(numVertices, false);
    std::stack<int> order;

    for (int v = 0; v < numVertices; ++v) {
        if (visited[v]) continue;

        std::stack<int> stack;
        stack.push(v);
        while (!stack.empty()) {
            int node = stack.top();

            if (!visited[node]) {
                visited[node] = true;
                if (edges.count(node)) {
                    for (int u : edges[node]) {
                        if (!visited[u]) {
                            stack.push(u);
                        }
                    }
                }
            } else {
                stack.pop();
                order.push(node);
            }
        }
    }

    std::fill(visited.begin(), visited.end(), false);
    int sccCount = 0;
    int maxSize = 0;

    while (!order.empty()) {
        int v = order.top();
        order.pop();

        if (visited[v]) continue;

        int size = 0;
        std::stack<int> stack;
        stack.push(v);

        while (!stack.empty()) {
            int node = stack.top();
            stack.pop();

            if (visited[node]) continue;
            visited[node] = true;
            size++;

            if (reverseEdges.count(node)) {
                for (int u : reverseEdges[node]) {
                    if (!visited[u]) {
                        stack.push(u);
                    }
                }
            }
        }

        maxSize = std::max(maxSize, size);
        sccCount++;
    }

    sccRatio = (numVertices > 0) ? static_cast<double>(maxSize) / numVertices : 0.0;
    return sccCount;
}

// std::vector<int> Graph::getLargestWCCVertices() {
//     std::unordered_set<int> visited;
//     std::vector<int> largestComponent;
//     int maxSize = 0;

//     for (int v = 0; v < numVertices; ++v) {
//         if (visited.find(v) == visited.end()) {
//             std::unordered_set<int> componentVisited;
//             bfsComponent(v, componentVisited);
//             if (componentVisited.size() > maxSize) {
//                 maxSize = componentVisited.size();
//                 largestComponent.assign(componentVisited.begin(), componentVisited.end());
//             }
//             visited.insert(componentVisited.begin(), componentVisited.end());
//         }
//     }

//     return largestComponent;
// }

// std::pair<int, int> Graph::bfsFurthestNode(int start) {
//     std::unordered_map<int, int> distances;
//     std::queue<int> q;
//     q.push(start);
//     distances[start] = 0;
//     int furthestNode = start;
//     int maxDistance = 0;

//     while (!q.empty()) {
//         int current = q.front();
//         q.pop();

//         if (edges.count(current)) {
//             for (int neighbor : edges.at(current)) {
//                 if (distances.find(neighbor) == distances.end()) {
//                     distances[neighbor] = distances[current] + 1;
//                     q.push(neighbor);
//                     if (distances[neighbor] > maxDistance) {
//                         maxDistance = distances[neighbor];
//                         furthestNode = neighbor;
//                     }
//                 }
//             }
//         }

//         if (reverseEdges.count(current)) {
//             for (int neighbor : reverseEdges.at(current)) {
//                 if (distances.find(neighbor) == distances.end()) {
//                     distances[neighbor] = distances[current] + 1;
//                     q.push(neighbor);
//                     if (distances[neighbor] > maxDistance) {
//                         maxDistance = distances[neighbor];
//                         furthestNode = neighbor;
//                     }
//                 }
//             }
//         }
//     }

//     return {furthestNode, maxDistance};
// }

// int Graph::estimateDiameterDoubleSweep() {
//     std::vector<int> vertices = getLargestWCCVertices();
//     if (vertices.empty()) return 0;

//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<> distr(0, vertices.size() - 1);
//     int randomStart = vertices[distr(gen)];

//     auto [nodeA, _] = bfsFurthestNode(randomStart);
//     auto [nodeB, diameter] = bfsFurthestNode(nodeA);

//     return diameter;
// }

// std::pair<int, int> Graph::estimateDiameterRandomPairs(int numPairs) {
//     std::vector<int> vertices = getLargestWCCVertices();
//     if (vertices.empty()) return {0, 0};

//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<> distr(0, vertices.size() - 1);

//     std::vector<int> distances;
//     for (int i = 0; i < numPairs; ++i) {
//         int u = vertices[distr(gen)];
//         int v = vertices[distr(gen)];
//         if (u == v) continue;

//         std::unordered_map<int, int> dist;
//         std::queue<int> q;
//         q.push(u);
//         dist[u] = 0;

//         bool found = false;
//         while (!q.empty() && !found) {
//             int current = q.front();
//             q.pop();

//             if (edges.count(current)) {
//                 for (int neighbor : edges.at(current)) {
//                     if (dist.find(neighbor) == dist.end()) {
//                         dist[neighbor] = dist[current] + 1;
//                         if (neighbor == v) {
//                             distances.push_back(dist[neighbor]);
//                             found = true;
//                             break;
//                         }
//                         q.push(neighbor);
//                     }
//                 }
//             }

//             if (found) break;

//             if (reverseEdges.count(current)) {
//                 for (int neighbor : reverseEdges.at(current)) {
//                     if (dist.find(neighbor) == dist.end()) {
//                         dist[neighbor] = dist[current] + 1;
//                         if (neighbor == v) {
//                             distances.push_back(dist[neighbor]);
//                             found = true;
//                             break;
//                         }
//                         q.push(neighbor);
//                     }
//                 }
//             }
//         }
//     }

//     if (distances.empty()) return {0, 0};

//     std::sort(distances.begin(), distances.end());
//     int diameter = *std::max_element(distances.begin(), distances.end());
//     int percentile90 = distances[static_cast<int>(0.9 * distances.size())];

//     return {diameter, percentile90};
// }

// std::vector<int> Graph::snowballSample(int targetSize) {
//     std::vector<int> vertices = getLargestWCCVertices();
//     if (vertices.empty() || targetSize >= vertices.size()) {
//         return vertices;
//     }

//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<> distr(0, vertices.size() - 1);

//     std::unordered_set<int> sampled;
//     std::queue<int> q;

//     // Начинаем с 2-3 случайных вершин
//     int initialNodes = std::min(3, static_cast<int>(vertices.size()));
//     for (int i = 0; i < initialNodes; ++i) {
//         int node = vertices[distr(gen)];
//         if (sampled.find(node) == sampled.end()) {
//             sampled.insert(node);
//             q.push(node);
//         }
//     }

//     while (!q.empty() && sampled.size() < targetSize) {
//         int current = q.front();
//         q.pop();

//         if (edges.count(current)) {
//             for (int neighbor : edges.at(current)) {
//                 if (sampled.find(neighbor) == sampled.end() && sampled.size() < targetSize) {
//                     sampled.insert(neighbor);
//                     q.push(neighbor);
//                 }
//             }
//         }

//         if (reverseEdges.count(current)) {
//             for (int neighbor : reverseEdges.at(current)) {
//                 if (sampled.find(neighbor) == sampled.end() && sampled.size() < targetSize) {
//                     sampled.insert(neighbor);
//                     q.push(neighbor);
//                 }
//             }
//         }
//     }

//     return std::vector<int>(sampled.begin(), sampled.end());
// }

// std::pair<int, int> Graph::estimateDiameterSnowballSample(int targetSize) {
//     std::vector<int> sampledVertices = snowballSample(targetSize);
//     if (sampledVertices.empty()) return {0, 0};

//     std::vector<int> distances;
//     for (size_t i = 0; i < sampledVertices.size(); ++i) {
//         int u = sampledVertices[i];
//         for (size_t j = i + 1; j < sampledVertices.size(); ++j) {
//             int v = sampledVertices[j];
//             if (u == v) continue;

//             std::unordered_map<int, int> dist;
//             std::queue<int> q;
//             q.push(u);
//             dist[u] = 0;

//             bool found = false;
//             while (!q.empty() && !found) {
//                 int current = q.front();
//                 q.pop();

//                 if (edges.count(current)) {
//                     for (int neighbor : edges.at(current)) {
//                         if (dist.find(neighbor) == dist.end()) {
//                             dist[neighbor] = dist[current] + 1;
//                             if (neighbor == v) {
//                                 distances.push_back(dist[neighbor]);
//                                 found = true;
//                                 break;
//                             }
//                             q.push(neighbor);
//                         }
//                     }
//                 }

//                 if (found) break;

//                 if (reverseEdges.count(current)) {
//                     for (int neighbor : reverseEdges.at(current)) {
//                         if (dist.find(neighbor) == dist.end()) {
//                             dist[neighbor] = dist[current] + 1;
//                             if (neighbor == v) {
//                                 distances.push_back(dist[neighbor]);
//                                 found = true;
//                                 break;
//                             }
//                             q.push(neighbor);
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     if (distances.empty()) return {0, 0};

//     std::sort(distances.begin(), distances.end());
//     int diameter = *std::max_element(distances.begin(), distances.end());
//     int percentile90 = distances[static_cast<int>(0.9 * distances.size())];

//     return {diameter, percentile90};
// }