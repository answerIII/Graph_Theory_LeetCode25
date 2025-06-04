#pragma once
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <stack>
#include <random>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <thread>
#include <mutex>
#include <string_view>
#include <algorithm>
#include <atomic>

struct DegreeStats {
    int minDeg;
    int maxDeg;
    double avgDeg;
};

struct DiamP90 { 
    int diameter;
    int p90;
};

struct ComponentInfo { 
    int size; 
    std::vector<int> vertices; 
};

struct Ordering {
    std::vector<int> order;             // вершины в порядке удаления
    std::vector<int> rank;              // rank[v] = индекс в порядке
    std::vector<std::vector<int>> fwd;  // fwd[u] = соседи u с более высоким рангом
};


class Graph {
    int numVertices;
    int numEdges;
    bool isLargeGraph;
    bool isDirected;
    double wccRatio = 0.0;
    double sccRatio = 0.0;
    long int maxDegreeVertex;
    mutable std::vector<int> parent;
    mutable std::vector<int> rank;
    std::vector<std::vector<long int>> edges;
    std::vector<std::vector<long int>> reverseEdges;
    std::vector<int> degrees;
    std::vector<int> degHist;
    std::vector<int> landmarks;
    std::vector<std::vector<int>> Ldist;
    bool landmarksReady = false;
    DegreeStats dStats{};  
    mutable long long triangleCount = -1;
    mutable Ordering ord;
    mutable ComponentInfo comp;

    //1A1
    int bfsComponent(int start, std::vector<bool>& visited);
    int countWeaklyConnectedComponentsBFS();
    int countWeaklyConnectedComponentsDSU();  
    void dsuInit(int n) const;
    int dsuFind(int x) const;
    void dsuUnion(int x, int y) const;
    void dfs1(int u, std::vector<bool>& vis, std::vector<int>& ord);
    void dfs2(int u, std::vector<bool>& vis, int& sz);

    //1A2    
    ComponentInfo buildLargestWCCConst() const;

    //1A3   
    void degeneracyOrder() const;
public:
    //Utils
    Graph();
    void loadFromFile(const std::string& path, const std::string& format);
    int getVertexCount() const;
    int getEdgeCount() const;
    double getWCCRatio() const { return wccRatio; }
    double getLargestSCCRatio() const { return sccRatio; }
    bool getDirected() const { return isDirected; }
    long long getTriangles() const { return triangleCount; }
    const std::vector<int>& degreeHistogram() const;

    //1A1
    int countWeaklyConnectedComponents();
    int countStronglyConnectedComponents();
    double getDensity() const;

    //1A2
    int exactDistance(int s, int t) const;
    DiamP90 estimateDiameterDoubleSweep() const;
    DiamP90 distanceStatsRandomPairs(int numPairs = 500) const;
    DiamP90 snowballDiameterAndP90(int sampleSize = 500, int iterations = 5) const;

    //1A3
    void countTriangles() const;
    double averageClusteringCoefficient() const;
    double globalClusteringCoefficient() const;

    //1A4
    double averageClusteringLargestWCC();

    //1A5
    DegreeStats getDegreeStats() const;

    //1B
    double ratioAfterRemoval(double perc, bool targetedByDeg, uint32_t seed = 42) const;

    //2
    enum class LandmarkSelect { Random, HighestDegree };
    void precomputeLandmarks(int k, LandmarkSelect sel = LandmarkSelect::Random, uint32_t seed = 42);
    int landmarkBasicDistance(int s, int t) const;    
};