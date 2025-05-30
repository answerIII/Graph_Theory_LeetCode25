#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <string>
#include <utility>

#include "progress.hpp"
#include "../libs.h"
#include "Node.h"

namespace stdr = std::ranges;

class Graph {
protected:
    std::vector<std::vector<int>> paths;
    std::vector<Node> nodes;
    std::vector<std::vector<int>> originalPaths ;
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
        while (weekComponents[0][randomIndex]->num == -1) {randomIndex = distrib(gen);}
        Node* r = weekComponents[0][randomIndex];
        std::pair<int, Node*> a = getFarthestVertexInsideWWC(r);
        std::pair<int, Node*> b = getFarthestVertexInsideWWC(a.second);

        seedNode = r;
        farthestNode = b.second;
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
        seedUsed = rd();

        //mark week component, work only with theLargestWCC nodes
        for (Node* node : weekComponents[0]) {
            if (node->num != -1) node->marked = true;
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
            maxDist = -1;
            meanDist = -1;
            medianDist = -1;
            usedPairs = 0;
            return;
        }
        usedPairs = distances.size();
        sort(distances.begin(), distances.end());
        int index90 = (int)(0.9 * distances.size());
        percentileB = distances[index90];

        maxDist = distances.back();
        meanDist = std::accumulate(distances.begin(), distances.end(), 0.0) / distances.size();
        if (distances.size() % 2 == 0) {
            medianDist = (distances[distances.size()/2 - 1] + distances[distances.size()/2]) / 2;
        }
        else {
            medianDist = distances[distances.size()/2];
        }
    }

    void init90PercentileC() {

        if (weekComponents.empty()) initWeekComponents();
        removeMarks();

        //create a snowball
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
    for (int u = 0; u < undirectedPaths.size(); ++u) {
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
            std::lock_guard<std::mutex> block(printLock);
            std::cout << "\rProgress: " << std::setw(3) << percent << "% completed" << std::flush;
            lastPrinted = percent;
        }
    };

    ProgressBlock block;
    ProgressStage& stage = block.create_stage(vertexCount);

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

            stage.arrive();
            // completed.fetch_add(1);
            // printProgress(vertexCount);
        }

        std::lock_guard<std::mutex> lock1(countMutex);
        trianglesCount += localTriangles;

        std::lock_guard<std::mutex> lock2(mapMutex);
        for (const auto& [node, count] : localMap) {
            trianglePerNode[node] += count;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = start + chunkSize;
        threads.emplace_back(worker, start, end);
    }

    for (auto& t : threads) t.join();
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

    void initLandmarksFarthestFirst() {
        landmarks.clear();
        if (undirectedPaths.empty()) initUndirectedPaths();
        size_t landmarksCount = 0;
        if (vertexCount > 100000) {
            landmarksCount = 200;
        } else if (vertexCount > 1000) {
            landmarksCount = 50;
        } else {
            landmarksCount = 5;
        }
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

        ProgressBlock block;
        ProgressStage& stage = block.create_stage(landmarksCount);

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
                        if (node.num == -1 ) continue;
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

                stage.arrive();
                // completedLandmarks.fetch_add(1);
                // printProgress(landmarksCount);
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

    void initLandmarksHeightDegrees()  {
        landmarks.clear();
        if (undirectedPaths.empty()) initUndirectedPaths();
        size_t landmarksCount = 0;
        if (vertexCount > 100000) {
            landmarksCount = 200;
        } else if (vertexCount > 1000) {
            landmarksCount = 50;
        } else {
            landmarksCount = 5;
        }
        landmarks.reserve(landmarksCount);

        std::mutex lock;
        std::mutex printLock;
        std::atomic<size_t> completedLandmarks = 0;

        //chose the nodes with most degrees
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
                std::cout << "\rProgress: " << std::setw(3) << percent << "% completed" << std::flush;
                lastPrinted = percent;
            }
        };

        ProgressBlock block;
        ProgressStage& stage = block.create_stage(landmarksCount);

        auto worker = [&](size_t times) {
            while (times-- != 0) {
                // Part 2: Landmark initialization
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

                stage.arrive();
                // completedLandmarks.fetch_add(1);
                // printProgress(landmarksCount);
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

    int getDistanceBetweenNodes(int num_u, int num_v) {
        if (landmarks.empty()) initLandmarksHeightDegrees();
        if (num_u >= nodes.size() || num_v >= nodes.size() || num_v*num_u < 0) { std::cout << "One of this nodes are absent in graph" << std::endl; return 0;}


        int minDistance = INT_MAX;
        bool pathNotFound = true;
        for (const auto& map : landmarks) {
            if (!(map.contains(num_u) && map.contains(num_v))) {continue;}
            if (map.at(num_u) + map.at(num_v) < minDistance) minDistance = map.at(num_u) + map.at(num_v); pathNotFound = false;
        }

        if (pathNotFound) {
            std::cout << "Path not found" << std::endl;
            return -1;
        }
        return minDistance;
    }

    DirectedGraph(Graph& graph)
    : Graph(graph) {}

    double getGlobalClusteringCoefficient() {
    if (undirectedPaths.empty())
        initUndirectedPaths();
    double triplets = 0;

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

};

class UndirectedGraph : public Graph {
//     std::vector<std::vector<Node*>>  strongComponents;
//     std::unordered_map<int, std::vector<int>> undirectedPaths;
//     std::vector<std::vector<Node*>>  weekComponents;
//
//     double density = 0;
//
//     void initDensity() {
//         double maxEdges = vertexCount * (vertexCount - 1) / 2.0;
//         density = edgesCount / maxEdges;
//     }
//     //
//     // void initComponents() {
//     //     for (auto& [num,node] : nodes) {
//     //         if (!node.marked) {
//     //             std::vector <Node*> component;
//     //             std::stack <Node*> stack;
//     //             stack.push(&node);
//     //             while (!stack.empty()) {
//     //                 Node* currentNode = stack.top(); stack.pop();
//     //                 component.push_back(currentNode);
//     //                 currentNode->marked = true;
//     //                 for (int neighborhood : paths[currentNode->num]) {
//     //                     if (nodes[neighborhood].marked != true) {
//     //                         stack.push(&nodes[neighborhood]);
//     //                     }
//     //                 }
//     //             }
//     //             strongComponents.push_back(component);
//     //         }
//     //     }
//     //     removeMarks();
//     //     std::ranges::sort(strongComponents, std::greater<>{});
//     // }
//
// public:
//     UndirectedGraph(Graph& graph): Graph(graph) {}
//
//     int getComponentsCount() {
//         if (strongComponents.empty()) initComponents();
//         return strongComponents.size();
//     }
//
//     double getDensity() {
//         if (density == 0) initDensity();
//         return density;
//     }
//
//     int getShareVertexInBeggestComponent() {
//         getComponentsCount();
//         return strongComponents[0].size() / vertexCount;
//     }
//
// // double getAverageClusteringCoefficient() {
// //     if (weekComponents.empty()) initWeekComponents(); // находим слабые компоненты
//
// //     const std::vector<Node*>& largestWCC = weekComponents[0];
//
// //     if (largestWCC.empty()) return 0.0;
//
// //     double totalClustering = 0.0;
//
// //     // Проходим по каждому узлу в компоненте
// //     for (Node* node : largestWCC) {
// //         int u = node->num;
// //         const std::vector<int>& neighbors = undirectedPaths[u];
//
// //         if (neighbors.size() < 2) {
// //             // Кластерный коэффициент равен 0, если у узла менее двух соседей
// //             continue;
// //         }
//
// //         int links = 0; // количество связей между соседями узла
// //         // Проверяем, есть ли ребро между каждой парой соседей
// //         for (size_t i = 0; i < neighbors.size(); ++i) {
// //             int vi = neighbors[i];
// //             for (size_t j = i + 1; j < neighbors.size(); ++j) {
// //                 int vj = neighbors[j];
//
// //                 // Проверяем, соединены ли vi и vj
// //                 const auto& vi_neighbors = undirectedPaths[vi];
// //                 if (std::find(vi_neighbors.begin(), vi_neighbors.end(), vj) != vi_neighbors.end()) {
// //                     ++links;
// //                 }
// //             }
// //         }
//
// //         // Возможное количество связей между соседями: C(k,2) = k*(k-1)/2
// //         int k = neighbors.size();
// //         double clusteringCoefficient = (2.0 * links) / (k * (k - 1));
// //         totalClustering += clusteringCoefficient;
// //     }
//
// //     return totalClustering / largestWCC.size();
// // }
//
//     // void initUndirectedPaths() {
//     //     undirectedPaths.reserve(paths.size());
//     //     for (auto& [u, neigh] : paths) {
//     //         for (int v : neigh) {
//     //             undirectedPaths[u].push_back(v);
//     //             undirectedPaths[v].push_back(u);
//     //         }
//     //     }
//     // }
//     //
//     //
//     // void initWeekComponents() {
//     //
//     //     if (undirectedPaths.empty()) { initUndirectedPaths(); }
//     //
//     //     for (auto& [num,node] : nodes) {
//     //         if (!node.marked) {
//     //             std::vector<Node*> component;
//     //             std::queue<Node*> queue;
//     //             queue.push(&node);
//     //             node.marked = true;
//     //             int nodeCount = 0;
//     //             while (!queue.empty()) {
//     //                 Node* currentNode = queue.front(); queue.pop();
//     //                 component.push_back(currentNode);
//     //                 ++nodeCount;
//     //                 for (int neighborhood : undirectedPaths[currentNode->num]) {
//     //                     if (nodes[neighborhood].marked != true) {
//     //                         nodes[neighborhood].marked = true;
//     //                         queue.push(&nodes[neighborhood]);
//     //                     }
//     //                 }
//     //             }
//     //             weekComponents.push_back(component);
//     //         }
//     //     }
//     //     std::sort(weekComponents.begin(), weekComponents.end(),
//     // [this](const std::vector<Node*>& a, const std::vector<Node*>& b) {
//     //     return a.size() > b.size();}
//     //     );
//     //
//     //     removeMarks();
//     // }
//
//
//  double getAverageClusteringCoefficient() {
//     std::atomic<double> totalCoefficient = 0.0;
//
//     // Инициализация наибольшей компоненты связности (для неориентированного графа — просто компонент связности)
//     initWeekComponents();
//
//     int countedVertices = weekComponents[0].size();
//
//     std::for_each(std::execution::par, weekComponents[0].begin(), weekComponents[0].end(), [&](Node* node) {
//         int u = node->num;
//
//         // Все соседи вершины u
//         const std::vector<int>& neighbors = paths[u];
//         int k = neighbors.size();
//
//         int linkCount = 0;
//
//         // Проверяем количество связей между соседями
//         for (int i = 0; i < k; ++i) {
//             for (int j = i + 1; j < k; ++j) {
//                 int ni = neighbors[i];
//                 int nj = neighbors[j];
//
//                 // Проверяем, связаны ли ni и nj
//                 linkCount += (stdr::contains(paths[ni], nj)) || (stdr::contains(paths[nj], ni));
//
//             }
//         }
//         if (k < 2) return;
//         // Кластерный коэффициент вершины
//         double Clu = (2.0 * linkCount) / (k * (k - 1));
//         totalCoefficient += Clu;
//     });
//
//     std::cout << "Total vertices in largest CC: " << countedVertices << std::endl;
//     std::cout << "Total clustering sum: " << totalCoefficient << std::endl;
//     std::cout<< "Average clustering coefficient: " << (countedVertices > 0 ? totalCoefficient / countedVertices : 0.0 ) << std::endl;
//     return countedVertices > 0 ? totalCoefficient / countedVertices : 0.0;
// }

};


#endif //GRAPH_H
