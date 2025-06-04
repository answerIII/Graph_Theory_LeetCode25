#include "Graph.h"

double Graph::averageClusteringLargestWCC() {
    if (comp.size == 0) return 0.0;

    Ordering ord;
    ord.order.reserve(comp.size);
    ord.rank.resize(numVertices, -1);
    ord.fwd.resize(numVertices);

    std::vector<int> degree(numVertices, 0);
    std::vector<std::vector<int>> undirected(numVertices);

    for (int u : comp.vertices) {
        auto it1 = edges[u].begin(), it2 = reverseEdges[u].begin();
        while (it1 != edges[u].end() || it2 != reverseEdges[u].end()) {
            int v;
            if (it2 == reverseEdges[u].end() || (it1 != edges[u].end() && *it1 < *it2)) v = *it1++;
            else if (it1 == edges[u].end() || *it2 < *it1) v = *it2++;
            else { v = *it1; ++it1; ++it2; }

            undirected[u].push_back(v);
        }
        std::sort(undirected[u].begin(), undirected[u].end());
        undirected[u].erase(std::unique(undirected[u].begin(), undirected[u].end()), undirected[u].end());
        degree[u] = undirected[u].size();
    }

    std::vector<int> bin(numVertices + 1), pos(numVertices), vert(numVertices);
    for (int u : comp.vertices) ++bin[degree[u]];
    for (int i = 1; i <= numVertices; ++i) bin[i] += bin[i - 1];

    for (int u : comp.vertices) {
        pos[u] = --bin[degree[u]];
        vert[pos[u]] = u;
    }

    std::vector<bool> removed(numVertices, false);
    for (int i = 0; i < (int)comp.size; ++i) {
        int u = vert[i];
        removed[u] = true;
        ord.rank[u] = i;
        ord.order.push_back(u);
        for (int v : undirected[u]) {
            if (!removed[v]) ord.fwd[u].push_back(v);
        }
    }

    std::vector<int> tri(numVertices, 0);
    std::vector<bool> mark(numVertices, false);
    for (int u : ord.order) {
        for (int v : ord.fwd[u]) mark[v] = true;
        for (int v : ord.fwd[u]) {
            for (int w : ord.fwd[v]) {
                if (mark[w]) {
                    ++tri[u]; ++tri[v]; ++tri[w];
                }
            }
        }
        for (int v : ord.fwd[u]) mark[v] = false;
    }

    double sum = 0.0;
    for (int u : comp.vertices) {
        int k = undirected[u].size();
        if (k < 2) continue;
        sum += (2.0 * tri[u]) / (k * (k - 1));
    }

    return sum / comp.size;
}
