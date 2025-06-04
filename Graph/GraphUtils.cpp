#include "Graph.h"
#include <set>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>

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

    std::unordered_set<long int> vertexSet;
    std::unordered_map<long int, std::vector<long int>> localEdges;
    std::unordered_map<long int, std::vector<long int>> localReverseEdges;

    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        if (++lineCount <= skipFirst) continue;
        if (line.empty() || line[0] == commentPrefix) continue;

        long int u = -1, v = -1;

        if (format == "csv") {
            size_t commaPos = line.find(',');
            if (commaPos == std::string::npos) {
                continue;
            }
            try {
                u = std::stol(line.substr(0, commaPos));
                v = std::stol(line.substr(commaPos + 1));
            } catch (...) {
                continue;
            }
        } else {
            std::istringstream iss(line);
            if (!(iss >> u >> v)) {
                continue;
            }
        }

        if (u < 0 || v < 0) { continue; }

        vertexSet.insert(u);
        vertexSet.insert(v);

        localEdges[u].push_back(v);
        localReverseEdges[v].push_back(u);
    }

    std::vector<long int> vertList(vertexSet.begin(), vertexSet.end());
    vertexSet.clear(); vertexSet.reserve(0);

    std::sort(vertList.begin(), vertList.end());
    numVertices = vertList.size();

    if (numVertices == 0) {
        edges.clear();
        reverseEdges.clear();
        degrees.clear();
        dStats.minDeg = 0;
        dStats.maxDeg = 0;
        dStats.avgDeg = 0.0;
        degHist.clear();
        maxDegreeVertex = -1;
        isLargeGraph = false;
        isDirected = false;
        return;
    }

    std::unordered_map<long int, int> idToIndex;
    idToIndex.reserve(numVertices);
    for (int i = 0; i < static_cast<int>(numVertices); ++i) {
        idToIndex[vertList[i]] = i;
    }

    edges.assign(numVertices, {});
    reverseEdges.assign(numVertices, {});
    numEdges = 0;

    for (const auto& [u0, neigh] : localEdges) {
        int u = idToIndex[u0];
        if (u < 0 || u >= static_cast<int>(numVertices)) { continue; }
        for (long int v0 : neigh) {
            auto it = idToIndex.find(v0);
            if (it == idToIndex.end()) { continue; }
            int v = it->second;
            if (v < 0 || v >= static_cast<int>(numVertices)) { continue; }
            edges[u].push_back(v);
            reverseEdges[v].push_back(u);
            ++numEdges;
        }
    }

    // Сортировка списков смежности
    for (size_t u = 0; u < numVertices; ++u) {
        std::sort(edges[u].begin(), edges[u].end());
        edges[u].erase(std::unique(edges[u].begin(), edges[u].end()), edges[u].end());

        std::sort(reverseEdges[u].begin(), reverseEdges[u].end());
        reverseEdges[u].erase(std::unique(reverseEdges[u].begin(), reverseEdges[u].end()), reverseEdges[u].end());
    }

    localEdges.clear();
    localReverseEdges.clear();

    degrees.assign(numVertices, 0);
    for (size_t u = 0; u < numVertices; ++u) {
        degrees[u] = static_cast<int>(edges[u].size() + reverseEdges[u].size());
    }

    dStats.minDeg = *std::min_element(degrees.begin(), degrees.end());
    dStats.maxDeg = *std::max_element(degrees.begin(), degrees.end());

    long long sumDeg = 0;
    for (int k : degrees) sumDeg += k;
    dStats.avgDeg = numVertices ? double(sumDeg) / numVertices : 0.0;

    degHist.assign(static_cast<size_t>(dStats.maxDeg + 1), 0);
    for (int k : degrees) {
        if (k >= 0 && k <= dStats.maxDeg) ++degHist[static_cast<size_t>(k)];
    }

    maxDegreeVertex = -1;
    int maxDeg = -1;
    for (size_t u = 0; u < numVertices; ++u) {
        int deg = static_cast<int>(edges[u].size() + reverseEdges[u].size());
        if (deg > maxDeg) {
            maxDeg = deg;
            maxDegreeVertex = static_cast<long int>(u);
        }
    }

    const size_t LARGE_THRESHOLD = 10000;
    isLargeGraph = (numEdges > LARGE_THRESHOLD);
    isDirected = (path.find("directed") != std::string::npos && path.find("undirected") == std::string::npos || path.find("digraph") != std::string::npos && path.size() > 8);
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