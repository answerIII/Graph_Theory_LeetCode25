#include "Graph.h"

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

long long Graph::countTriangles() const {
    std::vector<std::vector<int>> adj;
    buildUndirectedAdj(adj);

    long long triangles = 0;
    int n = numVertices;

    for(int u = 0; u < n; ++u){
        for(int v: adj[u]){
            if(v <= u) continue;
            for(int w: adj[v]){
                if(w <= v) continue;

                if(std::binary_search(adj[u].begin(), adj[u].end(), w)){
                    ++triangles;
                }
            }
        }
    }
    return triangles;
}

double Graph::averageClusteringCoefficient() const {
    std::vector<std::vector<int>> adj;
    buildUndirectedAdj(adj);

    double sumC = 0.0;
    int n = numVertices;

    for(int u = 0; u < n; ++u){
        int k = adj[u].size();
        if(k < 2) continue;

        int links = 0;
        for(int i = 0; i < k; ++i){
            int v = adj[u][i];
            for(int j = i+1; j < k; ++j){
                int w = adj[u][j];
                if(std::binary_search(adj[v].begin(), adj[v].end(), w))
                    ++links;
            }
        }

        double Cu = (2.0 * links) / (double(k) * double(k-1));
        sumC += Cu;
    }

    return sumC / n;
}

double Graph::globalClusteringCoefficient() const {
    long long T = countTriangles();
    std::vector<std::vector<int>> adj;
    buildUndirectedAdj(adj);

    long long triplets = 0;
    for(auto &nbrs : adj){
        long long k = nbrs.size();
        triplets += k * (k - 1) / 2;
    }
    if(triplets == 0) return 0.0;

    return double(3LL * T) / double(triplets);
}
