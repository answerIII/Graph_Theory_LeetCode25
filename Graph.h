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

    long long countTriangles() const;

    double averageClusteringCoefficient() const;

    double globalClusteringCoefficient() const;

    double averageClusteringLargestWCC();

    struct DegreeStats {
        int minDeg;
        int maxDeg;
        double avgDeg;
    };

    DegreeStats getDegreeStats() const;
    const std::vector<int>& degreeHistogram() const;

    double ratioAfterRemoval(double perc, bool targetedByDeg, uint32_t seed = 42) const;

    enum class LandmarkSelect { Random, HighestDegree };

    void precomputeLandmarks(int k, LandmarkSelect sel = LandmarkSelect::Random, uint32_t seed = 42);

    int landmarkBasicDistance(int s, int t) const;
    int exactDistance(int s, int t) const;

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

    std::vector<int> degrees;
    std::vector<int> degHist;
    DegreeStats dStats{};

    std::vector<int> landmarks;
    std::vector<std::vector<int>> Ldist;
    bool landmarksReady = false;
};