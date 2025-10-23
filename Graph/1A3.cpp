#include "Graph.h"

void Graph::degeneracyOrder() const {
    int n = numVertices;
    undirected_degrees.assign(n, 0);
    std::vector<bool> removed(n, false);
    std::vector<std::vector<int>> undirected(n);

    for (int u = 0; u < n; ++u) {
        auto it1 = edges[u].begin(), it2 = reverseEdges[u].begin();
        while (it1 != edges[u].end() || it2 != reverseEdges[u].end()) {
            if (it2 == reverseEdges[u].end() || (it1 != edges[u].end() && *it1 < *it2))
                undirected[u].push_back(*it1++);
            else if (it1 == edges[u].end() || *it2 < *it1)
                undirected[u].push_back(*it2++);
            else {
                undirected[u].push_back(*it1);
                ++it1; ++it2;
            }
        }
        std::sort(undirected[u].begin(), undirected[u].end());
        undirected[u].erase(std::unique(undirected[u].begin(), undirected[u].end()), undirected[u].end());
        undirected_degrees[u] = undirected[u].size();
    }

    ord.rank.resize(n);
    ord.fwd.resize(n);
    std::vector<int> bin(n + 1), pos(n), vert(n), order;

    for (int d : undirected_degrees) ++bin[d];
    for (int i = 1; i <= n; ++i) bin[i] += bin[i - 1];

    for (int u = 0; u < n; ++u) {
        pos[u] = --bin[undirected_degrees[u]];
        vert[pos[u]] = u;
    }

    for (int i = 0; i < n; ++i) {
        int u = vert[i];
        removed[u] = true;
        ord.rank[u] = i;
        order.push_back(u);
        for (int v : undirected[u]) {
            if (removed[v]) continue;
            ord.fwd[u].push_back(v);
        }
    }

    ord.order = std::move(order);
}

void Graph::countTriangles() const {
    degeneracyOrder(); 
    std::vector<bool> mark(numVertices, false);

    for (int u : ord.order) {
        for (int v : ord.fwd[u]) mark[v] = true;
        for (int v : ord.fwd[u]) {
            for (int w : ord.fwd[v]) {
                if (mark[w]) ++triangleCount;
            }
        }
        for (int v : ord.fwd[u]) mark[v] = false;
    }
}

double Graph::averageClusteringCoefficient() const {
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

    double sumC = 0.0;
    int count = 0;

    for (int u = 0; u < numVertices; ++u) {
        int k = undirected_degrees[u];
        if (k < 2) continue;
        double cu = (2.0 * tri[u]) / (k * (k - 1));
        sumC += cu;
        ++count;
    }

    return (numVertices == 0) ? 0.0 : sumC / numVertices;
}

double Graph::globalClusteringCoefficient() const {
    long long triplets = 0;

    for (int u = 0; u < numVertices; ++u) {
        int k = undirected_degrees[u];
        triplets += 1LL * k * (k - 1) / 2;
    }

    return (triplets == 0) ? 0.0 : (3.0 * triangleCount) / triplets;
}

double Graph::localClusteringCoefficient(int u) const {
    if (u < 0 || u >= numVertices) return 0.0;

    std::vector<int> nbr;
    auto it1 = edges[u].begin(), it2 = reverseEdges[u].begin();
    while (it1 != edges[u].end() || it2 != reverseEdges[u].end()) {
        if (it2 == reverseEdges[u].end() || (it1 != edges[u].end() && *it1 < *it2)) {
            nbr.push_back(*it1++);
        } else if (it1 == edges[u].end() || *it2 < *it1) {
            nbr.push_back(*it2++);
        } else {
            nbr.push_back(*it1);
            ++it1; ++it2;
        }
    }
    std::sort(nbr.begin(), nbr.end());
    nbr.erase(std::unique(nbr.begin(), nbr.end()), nbr.end());

    int k = static_cast<int>(nbr.size());
    if (k < 2) return 0.0;

    std::vector<char> mark(numVertices, 0);
    for (int v : nbr) mark[v] = 1;

    long long links = 0;

    for (int v : nbr) {
        auto a = edges[v].begin();
        auto b = reverseEdges[v].begin();
        while (a != edges[v].end() || b != reverseEdges[v].end()) {
            int w;
            if (b == reverseEdges[v].end() || (a != edges[v].end() && *a < *b)) {
                w = *a++;
            } else if (a == edges[v].end() || *b < *a) {
                w = *b++;
            } else {
                w = *a; ++a; ++b;
            }
            if (w == u) continue;
            if (w > v && mark[w]) ++links;
        }
    }

    return (2.0 * links) / (k * (k - 1));
}
