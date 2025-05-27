#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <stack>
#include <random>

class Graph {
public:
    Graph();

    void loadFromFile(const std::string& path, const std::string& format);
    int getVertexCount() const;
    int getEdgeCount() const;
    int countWeaklyConnectedComponents();
    int countStronglyConnectedComponents();
    double getDensity() const;
    double getWCCRatio() const { return wccRatio; }
    double getLargestSCCRatio() const { return sccRatio; }
    bool getDirected() const { return isDirected; }

    // int estimateDiameterDoubleSweep();
    // std::pair<int, int> estimateDiameterRandomPairs(int numPairs = 500);
    // std::pair<int, int> estimateDiameterSnowballSample(int targetSize = 500);
    // std::vector<int> getLargestWCCVertices();

private:
    int numVertices;
    int numEdges;
    bool isLargeGraph;
    bool isDirected;
    double wccRatio = 0.0;
    double sccRatio = 0.0;

    std::unordered_map<int, std::vector<int>> edges;
    std::unordered_map<int, std::vector<int>> reverseEdges;

    int bfsComponent(int start, std::unordered_set<int>& visited);
    int countWeaklyConnectedComponentsBFS();
    int countWeaklyConnectedComponentsDSU();

    // DSU helpers
    mutable std::vector<int> parent;
    mutable std::vector<int> rank;

    void dsuInit(int n) const;
    int dsuFind(int x) const;
    void dsuUnion(int x, int y) const;

    // Вспомогательные методы для BFS и вычисления расстояний
    std::pair<int, int> bfsFurthestNode(int start);
    std::vector<int> getRandomVertices(int count, const std::vector<int>& vertices);
    std::vector<int> snowballSample(int targetSize);
};