#include "Graph.h"

double Graph::getDensity() const {
    if (numVertices <= 1) return 0.0;

    double maxEdges = isDirected
        ? static_cast<double>(numVertices) * (numVertices - 1)
        : static_cast<double>(numVertices) * (numVertices - 1) / 2;

    return static_cast<double>(numEdges) / maxEdges;
}

int Graph::bfsComponent(int start, std::vector<bool>& visited) {
    std::queue<int> q;
    q.push(start);
    visited[start] = true;
    int size = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        ++size;

        for (int v : edges[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
        for (int v : reverseEdges[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }

    }
    return size;
}

int Graph::countWeaklyConnectedComponentsBFS() {
    std::vector<bool> visited(numVertices, false);
    int components = 0;
    int maxSize = 0;
    int size;

    for (int v = 0; v < numVertices; ++v) {
        if (!visited[v]) {
            size = bfsComponent(v, visited);
            components++;
        }
        maxSize = std::max(maxSize, size);
    }
    wccRatio = (numVertices > 0) ? static_cast<double>(maxSize) / numVertices : 0.0;
    return components;
}

void Graph::dsuInit(int n) const {
    parent.resize(n);
    rank.resize(n, 0);
    for (int i = 0; i < n; ++i)
        parent[i] = i;
}

int Graph::dsuFind(int x) const {
    if (parent[x] != x)
        parent[x] = dsuFind(parent[x]);
    return parent[x];
}

void Graph::dsuUnion(int x, int y) const {
    int rootX = dsuFind(x);
    int rootY = dsuFind(y);
    if (rootX == rootY) return;

    if (rank[rootX] < rank[rootY]) {
        parent[rootX] = rootY;
    } else if (rank[rootX] > rank[rootY]) {
        parent[rootY] = rootX;
    } else {
        parent[rootY] = rootX;
        rank[rootX]++;
    }
}

int Graph::countWeaklyConnectedComponentsDSU() {
    dsuInit(numVertices);

    for (int u = 0; u < static_cast<int>(numVertices); ++u) {
        for (int v : edges[u]) {
            dsuUnion(u, v);
        }
    }

    for (int u = 0; u < static_cast<int>(numVertices); ++u) {
        for (int v : reverseEdges[u]) {
            dsuUnion(u, v);
        }
    }

    std::unordered_map<int, int> componentSizes;
    componentSizes.reserve(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        ++componentSizes[dsuFind(i)];
    }

    int maxSize = 0;
    for (const auto& [_, size] : componentSizes)
        maxSize = std::max(maxSize, size);

    wccRatio = (numVertices > 0) ? static_cast<double>(maxSize) / numVertices : 0.0;
    return static_cast<int>(componentSizes.size());
}

void Graph::dfs1(int u, std::vector<bool>& vis, std::vector<int>& ord) {
    std::stack<std::pair<int,bool>> st;
    st.push({u, false});
    while (!st.empty()) {
        auto [x, processed] = st.top();
        st.pop();
        if (processed) {
            ord.push_back(x);
            continue;
        }
        if (vis[x]) continue;
        vis[x] = true;
        st.push({x, true});
        for (auto v_long : edges[x]) {
            int v = static_cast<int>(v_long);
            if (!vis[v]) st.push({v, false});
        }
    }
}

void Graph::dfs2(int u, std::vector<bool>& vis, int& sz) {
    std::stack<int> st;
    st.push(u);
    vis[u] = true;
    while (!st.empty()) {
        int x = st.top();
        st.pop();
        ++sz;
        for (auto v_long : reverseEdges[x]) {
            int v = static_cast<int>(v_long);
            if (!vis[v]) {
                vis[v] = true;
                st.push(v);
            }
        }
    }
}

int Graph::countWeaklyConnectedComponents() {
    return isLargeGraph ? countWeaklyConnectedComponentsDSU() : countWeaklyConnectedComponentsBFS();
}

int Graph::countStronglyConnectedComponents() {
    if (!this->isDirected) {
        return -1;
    }
    std::vector<bool> vis(numVertices, false);
    std::vector<int> order;
    order.reserve(numVertices);

    for (int i = 0; i < numVertices; ++i)
        if (!vis[i]) dfs1(i, vis, order);

    std::fill(vis.begin(), vis.end(), false);
    int sccCount = 0, maxSz = 0;
    for (int i = numVertices - 1; i >= 0; --i) {
        int u = order[i];
        if (!vis[u]) {
            int sz = 0;
            dfs2(u, vis, sz);
            sccCount++;
            maxSz = std::max(maxSz, sz);
        }
    }
    sccRatio = (numVertices > 0 ? double(maxSz) / numVertices : 0.0);
    return sccCount;
}