#include "Graph.h"

void Graph::precomputeLandmarks(int k, LandmarkSelect sel, uint32_t seed) {
    landmarks.clear();
    landmarks.reserve(k);
    int n = numVertices;

    if (sel == LandmarkSelect::HighestDegree) {
        std::vector<int> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(),
                  [&](int a, int b){
                      int da = edges[a].size() + reverseEdges[a].size();
                      int db = edges[b].size() + reverseEdges[b].size();
                      return da > db;
                  });
        for (int i = 0; i < k && i < n; ++i)
            landmarks.push_back(idx[i]);
    } else {
        std::mt19937_64 rng(seed);
        std::uniform_int_distribution<int> dist(0, n-1);
        std::unordered_set<int> used;
        while ((int)used.size() < std::min(k,n)) {
            used.insert(dist(rng));
        }
        landmarks.assign(used.begin(), used.end());
    }

    Ldist.assign(landmarks.size(), std::vector<int>(n, -1));
    for (size_t i = 0; i < landmarks.size(); ++i) {
        int src = landmarks[i];
        auto& d = Ldist[i];
        std::queue<int> q;
        d[src] = 0;
        q.push(src);

        while (!q.empty()) {
            int u = q.front(); q.pop();
            int du = d[u];
            for (int v : edges[u]) {
                if (d[v] < 0) { d[v] = du + 1; q.push(v); }
            }
            for (int v : reverseEdges[u]) {
                if (d[v] < 0) { d[v] = du + 1; q.push(v); }
            }
        }
    }

    landmarksReady = true;
}

int Graph::landmarkBasicDistance(int s, int t) const
{
    if (!landmarksReady) return -1;
    int best = std::numeric_limits<int>::max();
    for (size_t i = 0; i < landmarks.size(); ++i) {
        int ds = Ldist[i][s];
        int dt = Ldist[i][t];
        if (ds >= 0 && dt >= 0) {
            best = std::min(best, ds + dt);
        }
    }
    return best == std::numeric_limits<int>::max() ? -1 : best;
}

int Graph::landmarkBFSDistance(int s, int t) const {
    if (s == t) return 0;
    std::vector<int> dist(numVertices, -1);
    std::queue<int> q;
    dist[s] = 0;
    q.push(s);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        int d = dist[u];

        for (int v : edges[u]) {
            if (dist[v] == -1) {
                dist[v] = d + 1;
                if (v == t) return dist[v];
                q.push(v);
            }
        }

        for (int v : reverseEdges[u]) {
            if (dist[v] == -1) {
                dist[v] = d + 1;
                if (v == t) return dist[v];
                q.push(v);
            }
        }
    }
    return -1;
}

int Graph::distanceFromMaxVertex(int t) const {
    if (numVertices <= 0) return -1;
    if (t < 0 || t >= numVertices) return -1;
    int s = numVertices - 1;
    if (s == t) return 0;
    return landmarkBFSDistance(s, t);
}
