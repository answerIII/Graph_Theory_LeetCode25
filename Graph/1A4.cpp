#include "Graph.h"

Ordering Graph::degeneracyOrder(const std::vector<std::vector<int>>& adj) const {
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
    ComponentInfo comp = buildLargestWCCConst();
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