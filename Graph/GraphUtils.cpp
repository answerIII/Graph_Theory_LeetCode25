#include "Graph.h"

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

    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Cannot open file: " + path);

    int skipFirst = (format == "csv") ? 1 : (format == "mtx") ? 2 : 0;
    char commentPrefix = (format == "txt") ? '#' : '%';

    std::unordered_set<int> vertexSet;
    std::unordered_map<int, std::vector<int>> localEdges;
    std::unordered_map<int, std::vector<int>> localReverseEdges;

    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        if (++lineCount <= skipFirst) continue;
        if (line.empty() || line[0] == commentPrefix) continue;

        int u = -1, v = -1;

        if (format == "csv") {
            size_t commaPos = line.find(',');
            if (commaPos == std::string::npos) continue;
            try {
                u = std::stoi(line.substr(0, commaPos));
                v = std::stoi(line.substr(commaPos + 1));
            } catch (...) {
                continue;
            }
        } else {
            std::istringstream iss(line);
            if (!(iss >> u >> v)) continue;
        }

        if (u < 0 || v < 0) continue;

        vertexSet.insert(u);
        vertexSet.insert(v);

        localEdges[u].push_back(v);
        localReverseEdges[v].push_back(u);
    }

    std::vector<int> vertList(vertexSet.begin(), vertexSet.end());
    vertexSet.clear(); vertexSet.reserve(0);

    std::sort(vertList.begin(), vertList.end());
    numVertices = vertList.size();

    std::unordered_map<int, int> idToIndex;
    idToIndex.reserve(numVertices);
    for (int i = 0; i < static_cast<int>(numVertices); ++i) {
        idToIndex[vertList[i]] = i;
    }

    edges.assign(numVertices, {});
    reverseEdges.assign(numVertices, {});
    numEdges = 0;

    for (const auto& [u0, neigh] : localEdges) {
        int u = idToIndex[u0];
        for (int v0 : neigh) {
            int v = idToIndex[v0];
            edges[u].push_back(v);
            reverseEdges[v].push_back(u);
            ++numEdges;
        }
    }

    localEdges.clear();
    localReverseEdges.clear();

    degrees.resize(numVertices, 0);
    for (int u = 0; u < numVertices; ++u) {
        std::unordered_set<int> neigh(edges[u].begin(), edges[u].end());
        neigh.insert(reverseEdges[u].begin(), reverseEdges[u].end());
        degrees[u] = neigh.size();
    }

    dStats.minDeg = *std::min_element(degrees.begin(), degrees.end());
    dStats.maxDeg = *std::max_element(degrees.begin(), degrees.end());

    long long sumDeg = 0;
    for (int k : degrees) sumDeg += k;
    dStats.avgDeg = numVertices ? double(sumDeg) / numVertices : 0.0;

    degHist.assign(dStats.maxDeg + 1, 0);
    for (int k : degrees) ++degHist[k];

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

const std::vector<int>& Graph::degreeHistogram() const {
    return degHist;
}