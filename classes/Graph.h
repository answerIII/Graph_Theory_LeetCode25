#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <string>
#include <utility>

#include "../libs.h"
#include "Node.h"

namespace stdr = std::ranges;

class Graph {
protected:
    std::vector<std::vector<int>> paths;
    std::vector<Node> nodes;
    std::vector<std::vector<int>> originalPaths;
    std::vector<Node> originalNodes;
    int vertexCount = 0;
    int edgesCount = 0;

    void removeMarks() {
        for (Node& node : nodes) {
            node.marked = false;
        }
    }
public:
    Graph (const std::vector<Node>& nodes,
        const std::vector<std::vector<int>>& paths,
        int vertexCount, int edgesCount) {
        this->paths = paths;
        this->nodes = nodes;
        this->vertexCount = vertexCount;
        this->edgesCount = edgesCount;
        std::cout << "copied graph" << std::endl;
    }

    Graph (const Graph& graph) : paths(graph.paths), nodes(graph.nodes), vertexCount(graph.vertexCount), edgesCount(graph.edgesCount) {
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
    std::vector <std::vector<int>> transposePaths;
    std::vector <std::vector<int>> undirectedPaths;
    std::vector<std::vector<Node*>>  strongComponents;
    std::vector<std::vector<Node*>>  weekComponents;
    std::vector<std::unordered_map<int, int>> landmarks;
    std::vector<std::vector<std::unordered_map<int, int>>> weekTrees;

    double density = 0;
    int approximateDiameter = 0;
    int percentileB = 0;
    int percentileC = 0;
    int trianglesCount = -1;

    const size_t NUM_OF_THREADS = 12;

    std::unordered_map<int, int> trianglePerNode;

    Node* seedNode = nullptr;
    Node* farthestNode = nullptr;
    int maxDist = 0;
    double meanDist = 0.0;
    int medianDist = 0;
    int usedPairs = 0;
    int seedUsed = 0;
    int snowballSampleSize = 0;
    int snowballSamplePairs = 0;
    double snowballMean = 0.0;
    int snowballMedian = 0;
    int snowballP90 = 0;
    int snowballMax = 0;
    int snowballSeed = 0;


    void initWeekComponents() {

        if (undirectedPaths.empty()) { initUndirectedPaths(); }

        for (Node& node : nodes) {
            if (node.num == -1){continue;}
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
        undirectedPaths.resize(paths.size());
        std::vector<std::unordered_set<int>> uniqueNeighbours;
        uniqueNeighbours.resize(paths.size());
        for (int u = 0; u < paths.size(); ++u) {
            undirectedPaths[u].reserve(paths[u].size());
            for (int v : paths[u]) {
                if (!uniqueNeighbours[u].contains(v)) { undirectedPaths[u].push_back(v); uniqueNeighbours[u].insert(v); }
                if (!uniqueNeighbours[v].contains(u)) { undirectedPaths[v].push_back(u); uniqueNeighbours[v].insert(u); }
            }
        }
    }

    void initTransposePaths() {
        transposePaths.resize(paths.size());
        for (int u = 0; u < paths.size(); ++u) {
            for (int v : paths[u]) {
                transposePaths[v].push_back(u);
            }
        }
    }

    void initDensity() {
        long double maxEdges = vertexCount * (vertexCount - 1); // overflow???
        density = edgesCount / maxEdges;
    }

    void initStrongComponents() {
        //1 dfs
        std::vector<Node*> outVertexes;
        std::unordered_set<int> visited;
        for (Node& node : nodes) {
            if (node.num == -1) continue;
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
        if (weekComponents.empty() || weekComponents[0].empty()) return; // fail-safe

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, weekComponents[0].size() - 1);

        int randomIndex = distrib(gen);
        while (weekComponents[0][randomIndex]->num == -1) {
            randomIndex = distrib(gen);
        }

        Node* r = weekComponents[0][randomIndex];

        std::cout << "\rApproximate diameter: 0% [..........]" << std::flush;
        std::pair<int, Node*> a = getFarthestVertexInsideWWC(r);
        std::cout << "\rApproximate diameter: 50% [#####.....]" << std::flush;

        std::pair<int, Node*> b = getFarthestVertexInsideWWC(a.second);
        std::cout << "\rApproximate diameter: 100% [##########]\n" << std::flush;

        seedNode = r;
        farthestNode = b.second;
        approximateDiameter = b.first;
    }

    void init90PercentileB() {
    if (weekComponents.empty()) initWeekComponents();

    int samples = 500;
    int componentSize = weekComponents[0].size();
    if (componentSize < 500) samples = componentSize;

    std::vector<int> distances;
    distances.reserve(samples);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, componentSize - 1);
    seedUsed = rd();

    for (Node* node : weekComponents[0]) {
        if (node->num != -1) node->marked = true;
    }

    int numThreads = std::min(samples, 12);
    std::mutex distMutex;
    std::atomic<int> progressCounter(0);

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
        std::lock_guard<std::mutex> lock(distMutex);
        if (lengths.contains(v->num)) {
            distances.push_back(lengths[v->num]);
        }
    };

    std::vector<std::thread> threads;
    int perThread = samples / numThreads;
    int remainder = samples % numThreads;

    auto workerBatch = [&](int count) {
        for (int i = 0; i < count; ++i) {
            Node* u = weekComponents[0][dis(gen)];
            Node* v = weekComponents[0][dis(gen)];
            worker(u, v);

            int completed = ++progressCounter;
            int denominator =samples/100;
            if (denominator<=0) {
                denominator = 1;
            }
            if (completed % (denominator) == 0 || completed == samples) {
                std::cout << "\r90PercentileB: " << (completed * 100 / samples)
                          << "% (" << completed << "/" << samples << ")" << std::flush;
            }
        }
    };

    for (int i = 0; i < numThreads; ++i) {
        int count = perThread + (i < remainder ? 1 : 0);
        threads.emplace_back(workerBatch, count);
    }

    for (auto& t : threads) t.join();

    std::cout << "\r90PercentileB: 100% (" << samples << "/" << samples << ")\n"; // Завершающая строка

    if (distances.empty()) {
        percentileB = -1;
        maxDist = -1;
        meanDist = -1;
        medianDist = -1;
        usedPairs = 0;
        return;
    }

    usedPairs = distances.size();
    std::sort(distances.begin(), distances.end());

    int index90 = static_cast<int>(0.9 * distances.size());
    percentileB = distances[index90];

    maxDist = distances.back();
    meanDist = std::accumulate(distances.begin(), distances.end(), 0.0) / distances.size();
    if (distances.size() % 2 == 0) {
        medianDist = (distances[distances.size()/2 - 1] + distances[distances.size()/2]) / 2;
    } else {
        medianDist = distances[distances.size()/2];
    }
}

    void init90PercentileC() {
    if (weekComponents.empty()) initWeekComponents();
    removeMarks();

    int snowballSize = 500;
    int componentSize = weekComponents[0].size();
    if (componentSize < 500) snowballSize = componentSize;

    std::vector<Node*> snowball;
    snowball.reserve(snowballSize);

    std::random_device rd;
    std::mt19937 gen(rd());
    snowballSeed = rd();
    std::uniform_int_distribution<> dis(0, snowballSize - 1);
    int numThreads = std::min(snowballSize, 12);
    std::mutex distMutex;
    std::atomic<int> progressCounter(0);

    // Сбор снежка
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
        std::lock_guard<std::mutex> lock(distMutex);
        if (lengths.contains(v->num)) {
            distances.push_back(lengths[v->num]);
        }
    };

    auto workerBatch = [&](int count) {
        for (int i = 0; i < count; ++i) {
            Node* u = snowball[dis(gen)];
            Node* v = snowball[dis(gen)];
            worker(u, v);

            int denominator =snowballSize/100;
            if (denominator<=0) {
                denominator = 1;
            }

            int completed = ++progressCounter;
            if (completed % (denominator) == 0 || completed == snowballSize) {
                std::cout << "\r90PercentileC: " << (completed * 100 / snowballSize)
                          << "% (" << completed << "/" << snowballSize << ")" << std::flush;
            }
        }
    };

    std::vector<std::thread> threads;
    int perThread = snowballSize / numThreads;
    int remainder = snowballSize % numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int count = perThread + (i < remainder ? 1 : 0);
        threads.emplace_back(workerBatch, count);
    }

    for (auto& t : threads) t.join();

    std::cout << "\r90PercentileC: 100% (" << snowballSize << "/" << snowballSize << ")\n";

    std::sort(distances.begin(), distances.end());
    int index90 = static_cast<int>(0.9 * distances.size());
    percentileC = distances[index90];

    int num_pairs = distances.size();
    double mean_dist = std::accumulate(distances.begin(), distances.end(), 0.0) / num_pairs;
    int median_dist = distances[num_pairs / 2];
    int p90_dist = distances[index90];
    int max_dist = distances.back();
    int actual_snowball_size = snowball.size();

    snowballSampleSize = actual_snowball_size;
    snowballSamplePairs = num_pairs;
    snowballMean = mean_dist;
    snowballMedian = median_dist;
    snowballP90 = p90_dist;
    snowballMax = max_dist;
}

void initTrianglesCount() {
    trianglesCount = 0;
    trianglePerNode.clear();

    if (undirectedPaths.empty()) initUndirectedPaths();

    std::unordered_map<int, std::unordered_set<int>> adj;
    for (int u = 0; u < (int)undirectedPaths.size(); ++u) {
        for (int v : undirectedPaths[u]) {
            adj[u].insert(v);
            adj[v].insert(u);
        }
    }

    std::vector<int> nodesVec;
    for (const auto& [u, _] : adj) {
        nodesVec.push_back(u);
    }

    int vertexCount = static_cast<int>(nodesVec.size());
    int numThreads = 12;
    int totalNodes = vertexCount;
    int chunkSize = (totalNodes + numThreads - 1) / numThreads;

    std::mutex countMutex;
    std::mutex mapMutex;
    std::mutex printLock;

    std::atomic<int> completed{0};
    std::atomic<int> lastPrinted{-1};

    auto printProgress = [&](int total) {
        int done = completed.load();
        int percent = static_cast<int>((100.0 * done) / total);
        if (percent != lastPrinted.load()) {
            std::lock_guard<std::mutex> lock(printLock);
            std::cout << "\rTriangles Count: " << std::setw(3) << percent << "% " << std::flush;
            lastPrinted = percent;
        }
    };

    auto worker = [&](int start, int end) {
        int localTriangles = 0;
        std::unordered_map<int, int> localMap;

        for (int i = start; i < end && i < totalNodes; ++i) {
            int u = nodesVec[i];
            const auto& neighborsU = adj[u];
            std::vector<int> neighborsSorted(neighborsU.begin(), neighborsU.end());
            std::sort(neighborsSorted.begin(), neighborsSorted.end());

            for (size_t j = 0; j < neighborsSorted.size(); ++j) {
                int v = neighborsSorted[j];
                if (v <= u) continue;
                for (size_t k = j + 1; k < neighborsSorted.size(); ++k) {
                    int w = neighborsSorted[k];
                    if (w <= v) continue;
                    if (adj.at(v).count(w)) {
                        ++localTriangles;
                        ++localMap[u];
                        ++localMap[v];
                        ++localMap[w];
                    }
                }
            }

            ++completed;
            printProgress(totalNodes);
        }

        {
            std::lock_guard<std::mutex> lock1(countMutex);
            trianglesCount += localTriangles;
        }

        {
            std::lock_guard<std::mutex> lock2(mapMutex);
            for (const auto& [node, count] : localMap) {
                trianglePerNode[node] += count;
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = start + chunkSize;
        threads.emplace_back(worker, start, end);
    }

    for (auto& t : threads) t.join();

    // Завершаем прогресс-бар новой строкой
    {
        std::lock_guard<std::mutex> lock(printLock);
        std::cout << "\rTriangles Count: 100%          \n";
    }
}

    void backupOriginalGraph() {
        originalNodes = nodes;
        originalPaths = paths;
    }

    void resetToOriginal() {
        nodes = originalNodes;
        paths = originalPaths;

        undirectedPaths.clear();
        transposePaths.clear();
        strongComponents.clear();
        weekComponents.clear();

        density = 0;
        approximateDiameter = 0;
        percentileC = 0;
        percentileB = 0;
        trianglesCount = -1;

        vertexCount = (int)nodes.size();
        edgesCount = 0;
        for (const auto& adj : paths)
            edgesCount += (int)adj.size();
    }

    void removeNodes(int count, bool randomRemoving, const std::string& file) {
        std::ifstream inFile(file);

        if (randomRemoving)
        {
            int size = (int) paths.size() - 1;
            std::unordered_set<int> removedNodes;
            removedNodes.reserve(count);

            while (removedNodes.size() != count) {
                removedNodes.insert(nodes[nodes.size() - 1].num);
                nodes.pop_back();
            }

            ++size;
            paths.clear();
            nodes.clear();
            paths.resize(size);
            nodes.resize(size);
            edgesCount = 0;
            vertexCount = size - count;

            std::string line;
            while (std::getline(inFile, line)) {
                if (!(line.empty() || line[0] == '#'))  break;
            }

            int from;
            int to;
            std::istringstream iss(line);
            iss >> from >> to;
            if (!removedNodes.contains(from)) nodes[from] = Node(from,false);
            if (!removedNodes.contains(to)) nodes[to] = Node(to,false);
            if (!removedNodes.contains(to) && !removedNodes.contains(from)) { paths[from].push_back(to); ++edgesCount; }

            while (inFile >> from >> to) {
                if (!removedNodes.contains(from)) nodes[from] = Node(from,false);
                if (!removedNodes.contains(to)) nodes[to] = Node(to,false);
                if (!removedNodes.contains(to) && !removedNodes.contains(from)) { paths[from].push_back(to); ++edgesCount; }
            }
        }
        else {
            std::unordered_set<int> removedNodes;
            std::sort(nodes.begin(), nodes.end(),
                      [this](const Node& a, const Node& b) {
                          return paths[a.num].size() < paths[b.num].size();
                      });
            while (removedNodes.size() != count) {
                removedNodes.insert(nodes[nodes.size() - 1].num);
                nodes.pop_back();
            }

            size_t size = paths.size();
            paths.clear();
            nodes.clear();
            paths.resize(size);
            nodes.resize(size);
            edgesCount = 0;
            vertexCount = size - count;

            std::string line;
            while (std::getline(inFile, line)) {
                if (!(line.empty() || line[0] == '#'))  break;
            }

            int from;
            int to;
            std::istringstream iss(line);
            iss >> from >> to;
            if (!removedNodes.contains(from)) nodes[from] = Node(from,false);
            if (!removedNodes.contains(to)) nodes[to] = Node(to,false);
            if (!removedNodes.contains(to) && !removedNodes.contains(from)) { paths[from].push_back(to); ++edgesCount; }

            while (inFile >> from >> to) {
                if (!removedNodes.contains(from)) nodes[from] = Node(from,false);
                if (!removedNodes.contains(to)) nodes[to] = Node(to,false);
                if (!removedNodes.contains(to) && !removedNodes.contains(from)) {
                    paths[from].push_back(to); ++edgesCount;
                }
            }
        }

        undirectedPaths.clear();
        transposePaths.clear();
        strongComponents.clear();
        weekComponents.clear();
        density = 0;
        approximateDiameter = 0;
        percentileC = 0;
        percentileB = 0;
        trianglesCount = -1;
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

    void getVertexDegreeStats(const std::string& graph_id, const std::string& is_directed, std::ofstream& file) {
        std::unordered_map<int, long> in_degrees;
        std::unordered_map<int, long> out_degrees;
        for (int from = 0; from < paths.size(); ++from) {
            out_degrees[from] = paths[from].size();
            for (int to : paths[from]) {
                ++in_degrees[to];
            }
        }

        for (Node& node : nodes) {
            if (node.num == -1 ) continue;
            if (!out_degrees.count(node.num)) out_degrees[node.num] = 0;
            if (!in_degrees.count(node.num)) in_degrees[node.num] = 0;
        }

        std::unordered_map<int, long> total_degrees;
        for (Node& node : nodes) {
            if (node.num == -1 ) continue;
            total_degrees[node.num] = in_degrees[node.num] + out_degrees[node.num];
        }

        auto calc_stats = [&](const std::unordered_map<int, long>& deg_map, const std::string& deg_type) {
            std::vector<long> degrees;
            for (const auto& [_, deg] : deg_map) degrees.push_back(deg);

            long min_deg = degrees[0], max_deg = degrees[0];
            long double sum_deg = 0;

            for (long deg : degrees) {
                min_deg = std::min(min_deg, deg);
                max_deg = std::max(max_deg, deg);
                sum_deg += deg;
            }

            long double avg_deg = sum_deg / degrees.size();
            std::cout << deg_type << " - Min: " << min_deg
                      << ", Max: " << max_deg
                      << ", Avg: " << std::fixed << std::setprecision(2) << avg_deg << std::endl;

            std::map<long, long> degree_freq;
            for (long deg : degrees) degree_freq[deg]++;

            for (const auto& [degree, count] : degree_freq) {
                double pk = static_cast<double>(count) / degrees.size();
                double log_k = (degree > 0) ? log10(degree) : 0;
                double log_pk = (pk > 0) ? log10(pk) : 0;

                file << graph_id << ","
                     << is_directed << ","
                     << deg_type << ","
                     << degree << ","
                     << count << ","
                     << std::fixed << std::setprecision(6) << pk << ","
                     << log_k << ","
                     << log_pk << "\n";
            }
        };

        calc_stats(out_degrees, "out");
        calc_stats(in_degrees, "in");
        calc_stats(total_degrees, "total");
    }

    void initLandmarksFarthestFirst(int landmarksCount) {
        landmarks.clear();
        landmarksCount = std::min(vertexCount/2, landmarksCount);
        landmarks.reserve(landmarksCount);

        //first landmark
        {
            size_t maxDegree = 0;
            int vertex = 0;
            for (int u = 0; u < undirectedPaths.size(); ++u) {
                if (undirectedPaths[u].size() > maxDegree)  { maxDegree = undirectedPaths[u].size(); vertex = u; }
            }
            std::queue<Node*> queue;
            Node* landmarkNode = &nodes[vertex];
            landmarks.push_back(std::unordered_map<int, int>());
            landmarks[0][landmarkNode->num] = 0;
            queue.push(landmarkNode);
            landmarkNode->marked = true;
            while (!queue.empty()) {
                Node* currentNode = queue.front(); queue.pop();
                for (int neighborhood : undirectedPaths[currentNode->num]) {
                    if (nodes[neighborhood].marked == true) continue;
                    nodes[neighborhood].marked = true;
                    landmarks[0][neighborhood] = landmarks[0][currentNode->num] + 1;
                }
            }
            removeMarks();
            landmarkNode->marked = true;
        }

        std::mutex lock;
        std::mutex printLock;
        std::atomic<size_t> completedLandmarks = 1;

        auto printProgress = [&](size_t total) {
            size_t done = completedLandmarks.load();
            int percent = static_cast<int>((100.0 * done) / total);
            static std::atomic<int> lastPrinted{-1};
            if (percent != lastPrinted.load()) {
                std::lock_guard<std::mutex> block(printLock);
                std::cout << "\rProgress: " << std::setw(3) << percent << "% completed" << std::flush;
                lastPrinted = percent;
            }
        };
        auto worker = [&](size_t times) {
            while (times-- != 0) {
                // Part 1: Landmark selection
                Node* landmarkNode = nullptr;
                {
                    std::lock_guard<std::mutex> block(lock);

                    // Find max-min node
                    int maxMin = INT_MIN;
                    for (Node& node : nodes) {
                        if (node.marked) continue;

                        int currentMin = INT_MAX;
                        for (const auto& map : landmarks) {
                            if (map.contains(node.num)) {
                                currentMin = std::min(currentMin, map.at(node.num));
                            }
                        }

                        if (currentMin < INT_MAX && currentMin > maxMin) {
                            maxMin = currentMin;
                            landmarkNode = &node;
                        }
                    }
                    if (landmarkNode) landmarkNode->marked = true;
                }

                if (!landmarkNode) throw std::runtime_error("Landmark not found");
                // Part 2: Landmark initialization
                std::unordered_map<int,int> localMap;
                std::queue<Node*> queue;
                queue.push(landmarkNode);
                localMap[landmarkNode->num] = 0;

                while (!queue.empty()) {
                    Node* currentNode = queue.front(); queue.pop();
                    for (int neighborhood : undirectedPaths[currentNode->num]) {
                        if (!localMap.contains(neighborhood)) {
                            localMap[neighborhood] = localMap[currentNode->num] + 1;
                            queue.push(&nodes[neighborhood]);
                        }
                    }
                }

                {
                    std::lock_guard<std::mutex> block(lock);
                    landmarks.emplace_back(std::move(localMap));
                }

                completedLandmarks.fetch_add(1);
                printProgress(landmarksCount);
            }
        };

        std::vector<std::thread> workers;
        size_t remaining = landmarksCount - 1;
        for (size_t i = 0; i < NUM_OF_THREADS; ++i) {
            size_t tasks = remaining / (NUM_OF_THREADS - i);
            remaining -= tasks;
            if (tasks > 0) {
                workers.emplace_back(worker, tasks);
            }
        }

        for (auto& thread : workers) thread.join();
    }

    void initLandmarksHeightDegrees(int landmarksCount) {
    landmarks.clear();
    if (undirectedPaths.empty()) initUndirectedPaths();
    landmarksCount = std::min(vertexCount/2, landmarksCount);
    landmarks.reserve(landmarksCount);

    std::mutex lock;
    std::mutex printLock;
    std::atomic<size_t> completedLandmarks = 0;

    // chose the nodes with most degrees
    std::vector<int> nodeSorted;
    nodeSorted.reserve(nodes.size());
    for (Node& node : nodes) nodeSorted.push_back(node.num);
    std::sort(nodeSorted.begin(), nodeSorted.end(),
        [this](const int& a, const int& b) {
            return paths[a].size() < paths[b].size();
        });

    std::atomic<int> index = 0;

    auto printProgress = [&](size_t total) {
        size_t done = completedLandmarks.load();
        int percent = static_cast<int>((100.0 * done) / total);
        static std::atomic<int> lastPrinted{-1};
        if (percent != lastPrinted.load()) {
            std::lock_guard<std::mutex> block(printLock);
            std::cout << "\rLandmarks Height Degrees: " << std::setw(3) << percent << "%" << std::flush;
            lastPrinted = percent;
        }
    };

    // Используем worker с передачей количества задач на поток
    auto worker = [&](size_t times) {
        while (times-- != 0) {
            Node* landmarkNode = &nodes[nodeSorted[index++]];
            std::unordered_map<int,int> localMap;
            std::queue<Node*> queue;
            queue.push(landmarkNode);
            localMap[landmarkNode->num] = 0;

            while (!queue.empty()) {
                Node* currentNode = queue.front(); queue.pop();
                for (int neighborhood : undirectedPaths[currentNode->num]) {
                    if (!localMap.contains(neighborhood)) {
                        localMap[neighborhood] = localMap[currentNode->num] + 1;
                        queue.push(&nodes[neighborhood]);
                    }
                }
            }

            {
                std::lock_guard<std::mutex> block(lock);
                landmarks.emplace_back(std::move(localMap));
            }

            completedLandmarks.fetch_add(1);
            printProgress(landmarksCount);
        }
    };

    std::vector<std::thread> workers;
    size_t remaining = landmarksCount;
    const size_t NUM_OF_THREADS = std::min(12, (int)landmarksCount);
    for (size_t i = 0; i < NUM_OF_THREADS; ++i) {
        size_t tasks = remaining / (NUM_OF_THREADS - i);
        remaining -= tasks;
        if (tasks > 0) {
            workers.emplace_back(worker, tasks);
        }
    }

    for (auto& thread : workers) thread.join();

    // Завершаем прогресс-бар новой строкой
    {
        std::lock_guard<std::mutex> block(printLock);
        std::cout << "\rLandmarks Height Degrees: 100% completed           \n";
    }
}

    void initLandmarksForTreesGenerously() {
    if (weekComponents.empty()) initWeekComponents();

    weekTrees.clear();
    int completed = 0;
    size_t totalComponents = weekComponents.size();
    size_t total = 0;

    for (auto& component : weekComponents) {

        size_t sizeOfTree = component.size();
        size_t landmarksCount = 0;
        std::vector<std::unordered_map<int, int>> landmarks;
        if (sizeOfTree > 100000) {
            landmarksCount = 100;
        } else if (sizeOfTree > 10000){
            landmarksCount = 25;
        } else if (sizeOfTree > 1000){
            landmarksCount = 10;
        } else if (sizeOfTree > 100){
            landmarksCount = 5;
        } else {
            landmarksCount = 1;
        }
        total += landmarksCount;

        // Mark nodes inside component
        for (Node* node : component) {
            node->marked = true;
        }

        std::sort(component.begin(), component.end(),
    [&](Node* a, Node* b) {
        return undirectedPaths[a->num].size() > undirectedPaths[b->num].size();
    });

        std::mutex lock;
        std::mutex printLock;
        std::atomic<int> index = 0;

        auto printProgress = [&]() {
            int percent = static_cast<int>((100.0 * completed) / totalComponents);
            static std::atomic<int> lastPrinted{-1};
            if (percent != lastPrinted.load()) {
                std::lock_guard<std::mutex> block(printLock);
                std::cout << "\rProgress: " << std::setw(3) << percent << "% completed ("
                         << completed << "/" << totalComponents << " components)" << std::flush;
                lastPrinted = percent;
            }
        };

        auto worker = [&](size_t times) {
            while (times-- != 0) {
                Node* landmarkNode = component[index];
                index.fetch_add(1);
                std::unordered_map<int, int> localMap;
                std::queue<Node*> queue;
                queue.push(landmarkNode);
                localMap[landmarkNode->num] = 0;

                while (!queue.empty()) {
                    Node* currentNode = queue.front(); queue.pop();
                    for (int neighbor : undirectedPaths[currentNode->num]) {
                        if (!localMap.contains(neighbor)) {
                            localMap[neighbor] = localMap[currentNode->num] + 1;
                            queue.push(&nodes[neighbor]);
                        }
                    }
                }

                {
                    std::lock_guard<std::mutex> block(lock);
                    landmarks.emplace_back(std::move(localMap));
                }
            }
        };
        std::vector<std::thread> workers;
        size_t remaining = landmarksCount - 1;
        for (size_t i = 0; i < NUM_OF_THREADS; ++i) {
            size_t tasks = remaining / (NUM_OF_THREADS - i);
            remaining -= tasks;
            if (tasks > 0) {
                workers.emplace_back(worker, tasks);
            }
        }

        for (auto& thread : workers) thread.join();

        ++completed;
        printProgress();

        // Unmark the component
        for (Node* node : component) {
            node->marked = false;
        }
        weekTrees.push_back(landmarks);
    }

    std::cout << "\ntotal landmarks used:" << total << std::endl;
}

    void initLandmarksForTreesGreedy() {
    if (weekComponents.empty()) initWeekComponents();

    weekTrees.clear();
    int completed = 0;
    size_t totalComponents = weekComponents.size();
    size_t total = 0;

    for (auto& component : weekComponents) {

        size_t sizeOfTree = component.size();
        size_t landmarksCount = 0;
        std::vector<std::unordered_map<int, int>> landmarks;
        if (sizeOfTree > 1000000) {
            landmarksCount = 100;
        } else if (sizeOfTree > 500000) {
            landmarksCount = 50;
        } else if (sizeOfTree > 100000){
            landmarksCount = 15;
        } else if (sizeOfTree > 10000){
            landmarksCount = 10;
        } else if (sizeOfTree > 100){
            landmarksCount = 3;
        } else {
            landmarksCount = 1;
        }
        total += landmarksCount;

        // Mark nodes inside component
        for (Node* node : component) {
            node->marked = true;
        }

        std::sort(component.begin(), component.end(),
    [&](Node* a, Node* b) {
        return undirectedPaths[a->num].size() > undirectedPaths[b->num].size();
    });

        std::mutex lock;
        std::mutex printLock;
        std::atomic<int> index = 0;

        auto printProgress = [&]() {
            int percent = static_cast<int>((100.0 * completed) / totalComponents);
            static std::atomic<int> lastPrinted{-1};
            if (percent != lastPrinted.load()) {
                std::lock_guard<std::mutex> block(printLock);
                std::cout << "\rProgress: " << std::setw(3) << percent << "% completed ("
                         << completed << "/" << totalComponents << " components)" << std::flush;
                lastPrinted = percent;
            }
        };

        auto worker = [&](size_t times) {
            while (times-- != 0) {
                Node* landmarkNode = component[index];
                index.fetch_add(1);
                std::unordered_map<int, int> localMap;
                std::queue<Node*> queue;
                queue.push(landmarkNode);
                localMap[landmarkNode->num] = 0;

                while (!queue.empty()) {
                    Node* currentNode = queue.front(); queue.pop();
                    for (int neighbor : undirectedPaths[currentNode->num]) {
                        if (!localMap.contains(neighbor)) {
                            localMap[neighbor] = localMap[currentNode->num] + 1;
                            queue.push(&nodes[neighbor]);
                        }
                    }
                }

                {
                    std::lock_guard<std::mutex> block(lock);
                    landmarks.emplace_back(std::move(localMap));
                }
            }
        };
        std::vector<std::thread> workers;
        size_t remaining = landmarksCount - 1;
        for (size_t i = 0; i < NUM_OF_THREADS; ++i) {
            size_t tasks = remaining / (NUM_OF_THREADS - i);
            remaining -= tasks;
            if (tasks > 0) {
                workers.emplace_back(worker, tasks);
            }
        }

        for (auto& thread : workers) thread.join();

        ++completed;
        printProgress();

        // Unmark the component
        for (Node* node : component) {
            node->marked = false;
        }
        weekTrees.push_back(landmarks);
    }

    std::cout << "\ntotal landmarks used:" << total << std::endl;
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

    Node* getSeedNode() const { return seedNode; }
    Node* getFarthestNode() const { return farthestNode; }

    int get90PercentileB() {
        if (percentileB == 0) init90PercentileB();
        return percentileB;
    }

    int getMaxDist() const { return maxDist; }
    double getMeanDist() const { return meanDist; }
    int getMedianDist() const { return medianDist; }
    int getUsedPairs() const { return usedPairs; }
    int getSeedUsed() const { return seedUsed; }

    int get90PercentileC() {
        if (percentileC == 0) init90PercentileC();
        return percentileC;
    }
    int getSnowballSampleSize() const { return snowballSampleSize; }
    int getSnowballSeed() const { return snowballSeed; }
    int getSnowballSamplePairs() const { return snowballSamplePairs; }
    double getSnowballMean() const { return snowballMean; }
    int getSnowballMedian() const { return snowballMedian; }
    int getSnowballP90() const { return snowballP90; }
    int getSnowballMax() const { return snowballMax; }

    size_t getCountNodesInLargestWCC() {
        if (weekComponents.empty()) initWeekComponents();
        return weekComponents[0].size();
    }

    size_t getCountNodesInLargestSCC() {
        if (weekComponents.empty()) initStrongComponents();
        return strongComponents[0].size();
    }

    int getTrianglesCount() {
        if (trianglesCount == -1) initTrianglesCount();
        return trianglesCount;
    }

    void removeRandomNodes(int percent, const std::string& file) {
        int count = percent*vertexCount / 100;
        this->backupOriginalGraph();
        if (count > vertexCount) std::cout << vertexCount << " vertices are less than " << count << std::endl;

        removeNodes(count, true, file);
    }

    void removeMostDegreesNodes(int percent, const std::string& file) {
        int count = percent*vertexCount / 100;
        if (count > vertexCount) std::cout << vertexCount << " vertices are less than " << count << std::endl;

        removeNodes(count, false, file);
    }

    void getVertexDegree(const std::string& graph_id, const std::string& is_directed,  std::ofstream& file) {
        getVertexDegreeStats(graph_id, is_directed, file);
    }

    int getDistanceBetweenNodes(int num_u, int num_v, int landmarksCount) {
    if (landmarks.empty()) initLandmarksHeightDegrees(landmarksCount);
    if (num_u >= nodes.size() || num_v >= nodes.size() || num_v * num_u < 0) {
        std::cout << "One of these nodes is absent in graph" << std::endl;
        return 0;
    }

    int minDistance = INT_MAX;
    bool pathNotFound = true;

    int processed = 0;


    for (const auto& map : landmarks) {
        if (!(map.contains(num_u) && map.contains(num_v))) {
            ++processed;
            continue;
        }
        int dist = map.at(num_u) + map.at(num_v);
        if (dist < minDistance) {
            minDistance = dist;
            pathNotFound = false;
        }

        ++processed;
    }


    if (pathNotFound) {
        std::cout << "Path not found" << std::endl;
        return -1;
    }

    return minDistance;
}

    int getMinDistanceBetweenNodes(int num_u, int num_v) {
        if (num_u >= nodes.size() || num_v >= nodes.size() || num_v * num_u < 0) {
            std::cout << "One of these nodes is absent in graph" << std::endl;
            return 0;
        }
        if (undirectedPaths.empty()) initUndirectedPaths();

        Node* u = &nodes[num_u];
        std::queue<Node*> queue;
        std::vector<int> distances;
        distances.resize(nodes.size());
        queue.push(u);
        u->marked = true;
        distances[u->num] = 0;

        while (!queue.empty()) {
            Node* currentNode = queue.front(); queue.pop();
            if (currentNode->num == num_v) return distances[currentNode->num];
            for (int neighborhood : undirectedPaths[currentNode->num]) {
                if (!nodes[neighborhood].marked) {
                    nodes[neighborhood].marked = true;
                    distances[neighborhood] = distances[currentNode->num] + 1;
                    queue.push(&nodes[neighborhood]);
                }
            }
        }
        removeMarks();
    }

    DirectedGraph(Graph& graph)
    : Graph(graph) {}

    double getGlobalClusteringCoefficient() {
    if (undirectedPaths.empty()) initUndirectedPaths();
    if (trianglesCount == -1) initTrianglesCount();

    long double triplets = 0;

    for (int u = 0; u < undirectedPaths.size(); ++u) {
        size_t k = undirectedPaths[u].size();
        triplets += ((k * (k - 1)) / 2.0);
    }
    //std::cout<<"Num of triplets:" << triplets<<std::endl;
    if (triplets == 0) return 0.0;
    

    return (3.0 * trianglesCount) / triplets;
}

    double getAverageClusteringCoefficient() {
        if (trianglePerNode.empty()) initTrianglesCount();
        if (undirectedPaths.empty()) initUndirectedPaths();

        int totalNodes = nodes.size();

        int numThreads = 12;
        int chunkSize = (totalNodes + numThreads - 1) / numThreads;

        std::vector<double> threadTotals(numThreads, 0.0);
        std::vector<int> threadCounts(numThreads, 0);

        auto worker = [&](int threadId, int start, int end) {
            try {
                double localTotal = 0.0;
                int localCount = 0;

                for (int i = start; i < end && i < totalNodes; ++i) {
                    int u = nodes[i].num;
                    const auto& neighbors = undirectedPaths[u];
                    size_t k = neighbors.size();
                    if (k < 2) continue;

                    // Catch potential missing entry
                    if (!trianglePerNode.count(u)) {
                        continue;
                    }

                    int t = trianglePerNode.at(u);
                    double denominator = k * (k - 1.0);
                    if (denominator < 1e-9) continue;

                    double clu = (2.0 * t) / denominator;
                    localTotal += clu;
                    localCount++;
                }

                threadTotals[threadId] = localTotal;
                threadCounts[threadId] = localCount;
            } catch (const std::exception& ex) {
                std::cerr << "Exception in thread " << threadId << ": " << ex.what() << "\n";
            }
        };


        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            int start = i * chunkSize;
            int end = start + chunkSize;
            threads.emplace_back(worker, i, start, end);
        }

        for (auto& t : threads) t.join();

        double total = std::accumulate(threadTotals.begin(), threadTotals.end(), 0.0);
        int count = std::accumulate(threadCounts.begin(), threadCounts.end(), 0);

        return count > 0 ? total / count : 0.0;
    }

    double getAverageClusteringCoefficientOfWCC(const std::string& graph_id, const std::string& is_directed, std::ofstream& file) {
    if (trianglePerNode.empty()) initTrianglesCount();
    if (undirectedPaths.empty()) initUndirectedPaths();
    if (weekComponents.empty()) initWeekComponents();

    const std::vector<Node*>& largestWCC = weekComponents[0];

    double total = 0.0;
    int count = 0;

    for (Node* node : largestWCC) {
        int u = node->num;
        int k = undirectedPaths[u].size();
        if (k < 2) continue;

        int T = trianglePerNode[u];
        double clu = (2.0 * T) / (k * (k - 1));
        file << graph_id << ","
              << is_directed << ","
              << u << ","
              << k << ","
              << T << ","
              << clu << "\n";
        total += clu;
        ++count;
    }
        file << graph_id << ","
             << is_directed << ","
             << "Number of vertexes in the WCC:" << ","
             << weekComponents[0].size() << ","
             << "Average Clustering Coef of the WCC:" << ","
             << (count > 0 ? total / count : 0.0) << ",";
    std::cout<<"Number of vertexes in the WCC:" << weekComponents[0].size() <<std::endl;
    std::cout<<"Average Clustering Coef of the WCC: "<< (count > 0 ? total / count : 0.0) <<std::endl;
    return count > 0 ? total / count : 0.0;
}

    int getDistanceBetweenNodesLTC(int num_u, int num_v) {
        if (weekTrees.empty()) initLandmarksForTreesGenerously();
        if (num_u >= nodes.size() || num_v >= nodes.size() || num_v * num_u < 0) {
            std::cout << "One of these nodes is absent in graph" << std::endl;
            return 0;
        }

        int minDistance = INT_MAX;
        bool pathNotFound = true;

        int processed = 0;


        for (const auto&  landmarksVec : weekTrees) {
            for (const auto& map : landmarksVec) {
                if (!(map.contains(num_u) && map.contains(num_v))) {
                    continue;
                }

                int dist = map.at(num_u) + map.at(num_v);
                if (dist < minDistance) {
                    minDistance = dist;
                    pathNotFound = false;
                }
            }
            ++processed;
        }


        if (pathNotFound) {
            std::cout << "Path not found" << std::endl;
            return -1;
        }

        return minDistance;
    }

};



#endif //GRAPH_H
