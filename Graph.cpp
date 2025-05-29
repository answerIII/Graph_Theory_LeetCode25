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
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <atomic>

Graph::Graph()
    : numVertices(0)
    , numEdges(0)
    , isLargeGraph(false)
    , maxDegreeVertex(-1)
{}

void Graph::loadFromFile(const std::string& path, const std::string& format) {
    numVertices = numEdges = 0;
    isLargeGraph = false;
    maxDegreeVertex = -1;
    edges.clear();
    reverseEdges.clear();

    FILE* file = fopen(path.c_str(), "rb");
    if (!file) throw std::runtime_error("Cannot open file: " + path);

    constexpr long long BUFFER_SIZE = 1024 * 1024 * 1024; // 1GB
    std::vector<char> buffer(BUFFER_SIZE);
    size_t bytesRead = fread(buffer.data(), 1, BUFFER_SIZE, file);
    fclose(file);
    if (bytesRead == 0) return;

    int skipFirst = (format == "csv") ? 1 : (format == "mtx") ? 2 : 0;
    char commentPrefix = (format == "txt") ? '#' : '%';

    std::vector<std::string_view> lines;
    lines.reserve(1000000);
    for (size_t start = 0, i = 0; i < bytesRead; ++i) {
        if (buffer[i] == '\n' || buffer[i] == '\r') {
            if (i > start) lines.emplace_back(&buffer[start], i - start);
            start = i + 1;
        }
    }
    if (!lines.empty() && lines.back().size() == 0) lines.pop_back();

    if (skipFirst > 0 && lines.size() > static_cast<size_t>(skipFirst)) {
        lines.erase(lines.begin(), lines.begin() + skipFirst);
    }

    size_t nThreads = std::thread::hardware_concurrency();
    if (nThreads == 0) nThreads = 4;

    std::unordered_set<int> vertexSet;
    std::mutex vertexMutex;

    std::vector<std::unordered_map<int,std::vector<int>>> localEdges(nThreads);
    std::vector<std::unordered_map<int,std::vector<int>>> localReverseEdges(nThreads);

    auto worker = [&](size_t tid, size_t begin, size_t end) {
        auto& locE = localEdges[tid];
        auto& locR = localReverseEdges[tid];

        for (size_t i = begin; i < end; ++i) {
            auto& line = lines[i];
            if (line.empty() || line[0] == commentPrefix) continue;

            const char* ptr = line.data();
            char* endPtr;
            int u = std::strtol(ptr, &endPtr, 10);
            if (endPtr == ptr) continue;
            while (*endPtr==' '||*endPtr==','||*endPtr=='\t') ++endPtr;
            char* endPtr2;
            int v = std::strtol(endPtr, &endPtr2, 10); 
            if (endPtr2==endPtr) continue;
            if (u<0||v<0) continue;

            {
                std::lock_guard<std::mutex> lk(vertexMutex);
                vertexSet.insert(u);
                vertexSet.insert(v);
            }
            locE[u].push_back(v);
            locR[v].push_back(u);
        }
    };

    std::vector<std::thread> threads;
    size_t blockSize = (lines.size() + nThreads - 1) / nThreads;
    for (size_t t = 0; t < nThreads; ++t) {
        size_t begin = t * blockSize;
        size_t end = std::min(lines.size(), begin + blockSize);
        threads.emplace_back(worker, t, begin, end);
    }
    for (auto& th : threads) th.join();

    std::vector<int> vertList(vertexSet.begin(), vertexSet.end());
    std::sort(vertList.begin(), vertList.end());
    numVertices = vertList.size();

    std::unordered_map<int,int> idToIndex;
    idToIndex.reserve(numVertices);
    for (int i = 0; i < static_cast<int>(numVertices); ++i) {
        idToIndex[vertList[i]] = i;
    }

    edges.assign(numVertices, {});
    reverseEdges.assign(numVertices, {});

    numEdges = 0;
    for (size_t t = 0; t < nThreads; ++t) {
        for (auto& kv : localEdges[t]) {
            int u0 = kv.first;
            int u = idToIndex[u0];
            for (int v0 : kv.second) {
                int v = idToIndex[v0];
                edges[u].push_back(v);
                reverseEdges[v].push_back(u);
                ++numEdges;
            }
        }
    }

    maxDegreeVertex = -1;
    int maxDeg = -1;
    for (int u = 0; u < static_cast<int>(numVertices); ++u) {
        int deg = edges[u].size() + reverseEdges[u].size();
        if (deg > maxDeg) {
            maxDeg = deg;
            maxDegreeVertex = u;
        }
    }

    const size_t LARGE_THRESHOLD = 10000; 
    isLargeGraph = (numEdges > LARGE_THRESHOLD);
    isDirected = (path.find("directed") != std::string::npos && path.find("undirected") == std::string::npos);
}

int Graph::getVertexCount() const {
    return numVertices;
}

int Graph::getEdgeCount() const {
    return numEdges;
}

int Graph::bfsComponent(int start, std::vector<bool>& visited) {
    std::queue<int> q;
    q.push(start);
    visited[start] = true;
    int size = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        ++size;

        for (int v : edges[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
        for (int v : reverseEdges[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }

    }
    return size;
}

int Graph::countWeaklyConnectedComponentsBFS() {
    std::vector<bool> visited(numVertices, false);
    int components = 0;
    int maxSize = 0;
    int size;

    for (int v = 0; v < numVertices; ++v) {
        if (!visited[v]) {
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

    for (int u = 0; u < static_cast<int>(numVertices); ++u) {
        for (int v : edges[u]) {
            dsuUnion(u, v);
        }
    }

    for (int u = 0; u < static_cast<int>(numVertices); ++u) {
        for (int v : reverseEdges[u]) {
            dsuUnion(u, v);
        }
    }

    std::unordered_map<int, int> componentSizes;
    componentSizes.reserve(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        ++componentSizes[dsuFind(i)];
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

void Graph::dfs1(int u, std::vector<bool>& vis, std::vector<int>& ord) {
    vis[u] = true;
    for (int v : edges[u])
        if (!vis[v]) dfs1(v, vis, ord);
    ord.push_back(u);
}
void Graph::dfs2(int u, std::vector<bool>& vis, int& sz) {
    vis[u] = true;
    sz++;
    for (int v : reverseEdges[u])
        if (!vis[v]) dfs2(v, vis, sz);
}

int Graph::countStronglyConnectedComponents() {
    std::vector<bool> vis(numVertices, false);
    std::vector<int> order;
    order.reserve(numVertices);

    for (int i = 0; i < numVertices; ++i)
        if (!vis[i]) dfs1(i, vis, order);

    std::fill(vis.begin(), vis.end(), false);
    int sccCount = 0, maxSz = 0;
    for (int i = numVertices - 1; i >= 0; --i) {
        int u = order[i];
        if (!vis[u]) {
            int sz = 0;
            dfs2(u, vis, sz);
            sccCount++;
            maxSz = std::max(maxSz, sz);
        }
    }
    sccRatio = (numVertices > 0 ? double(maxSz) / numVertices : 0.0);
    return sccCount;
}

Graph::ComponentInfo Graph::buildLargestWCC() {
    dsuInit(numVertices);
    for (int u = 0; u < numVertices; ++u)
        for (int v : edges[u]) dsuUnion(u,v);
    for (int u = 0; u < numVertices; ++u)
        for (int v : reverseEdges[u]) dsuUnion(u,v);

    std::vector<int> root(numVertices);
    std::unordered_map<int,int> sz;
    sz.reserve(numVertices);

    for (int v = 0; v < numVertices; ++v){
        root[v] = dsuFind(v);
        ++sz[root[v]];
    }

    int bigRoot = std::max_element(sz.begin(), sz.end(),
                    [](auto &a,auto &b){return a.second<b.second;})->first;

    ComponentInfo info;
    info.size = sz[bigRoot];
    info.vertices.reserve(info.size);
    for (int v = 0; v < numVertices; ++v)
        if (root[v]==bigRoot) info.vertices.push_back(v);

    wccRatio = double(info.size)/numVertices;
    return info;
}

void Graph::buildUndirectedAdj(std::vector<std::vector<int>>& adj) const {
    adj.assign(numVertices,{});
    for (int u = 0; u < numVertices; ++u){
        for (int v : edges[u]) if (u != v) { adj[u].push_back(v); adj[v].push_back(u); }
        for (int v : reverseEdges[u]) if (u != v) { adj[u].push_back(v); adj[v].push_back(u); }
    }
    for (auto &vec:adj){
        std::sort(vec.begin(),vec.end());
        vec.erase(std::unique(vec.begin(),vec.end()),vec.end());
    }
}

Graph::Ordering Graph::degeneracyOrder(const std::vector<std::vector<int>>& adj) const {
    const int n = adj.size();
    std::vector<int> order(n), rank(n), deg(n);
    for (int i = 0; i < n; ++i) deg[i] = adj[i].size();

    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
              [&](int a, int b) { return deg[a] < deg[b]; });
    for (int i = 0; i < n; ++i) rank[order[i]] = i;

    std::vector<std::vector<int>> fwd(n);
    for (int u = 0; u < n; ++u)
        for (int v : adj[u])
            if (rank[u] < rank[v]) fwd[u].push_back(v);

    for (int u = 0; u < n; ++u)
        std::sort(fwd[u].begin(), fwd[u].end(),
                  [&](int a, int b) { return rank[a] < rank[b]; });

    return {std::move(rank), std::move(fwd)};
}

double Graph::averageClusteringLargestWCC(){
    ComponentInfo comp = buildLargestWCC();
    if(comp.size==0) return 0.0;

    std::vector<std::vector<int>> adj;
    buildUndirectedAdj(adj);
    Ordering ord = degeneracyOrder(adj);

    std::vector<int> tri(numVertices,0);
    for(int u:comp.vertices){
        for(int v:ord.fwd[u]){
            auto it1=ord.fwd[u].begin(), it2=ord.fwd[v].begin();
            while(it1!=ord.fwd[u].end() && it2!=ord.fwd[v].end()){
                if(*it1==*it2){
                    ++tri[u]; ++tri[v]; ++tri[*it1];
                    ++it1; ++it2;
                } else if(ord.rank[*it1] < ord.rank[*it2]) ++it1;
                else ++it2;
            }
        }
    }

    double sum = 0.0;
    for(int u:comp.vertices){
        int k = adj[u].size();
        if(k<2) continue;
        sum += (2.0*tri[u]) / (k*(k-1));
    }
    return sum/comp.size;
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