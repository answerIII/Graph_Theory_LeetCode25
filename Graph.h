#pragma once
#include <string>
#include <sstream>
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

    double averageClusteringLargestWCC();

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
    long int maxDegreeVertex;

    std::vector<std::vector<long int>> edges;
    std::vector<std::vector<long int>> reverseEdges;

    int bfsComponent(int start, std::vector<bool>& visited);
    int countWeaklyConnectedComponentsBFS();
    int countWeaklyConnectedComponentsDSU();

    // DSU helpers
    mutable std::vector<int> parent;
    mutable std::vector<int> rank;

    void dsuInit(int n) const;
    int dsuFind(int x) const;
    void dsuUnion(int x, int y) const;
    void dfs1(int u, std::vector<bool>& vis, std::vector<int>& ord);
    void dfs2(int u, std::vector<bool>& vis, int& sz);

    struct ComponentInfo { int size; std::vector<int> vertices; };

    ComponentInfo buildLargestWCC();
    void buildUndirectedAdj(std::vector<std::vector<int>>&) const;
    struct Ordering { std::vector<int> rank; std::vector<std::vector<int>> fwd; };
    Ordering degeneracyOrder(const std::vector<std::vector<int>>&) const;
};