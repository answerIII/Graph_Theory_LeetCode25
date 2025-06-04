#include "Graph.h"

int Graph::exactDistance(int s, int t) const {
    if (s < 0 || s >= numVertices || t < 0 || t >= numVertices) 
        return -1;
    if (s == t) return 0;

    std::vector<int> dist(numVertices, -1);
    std::queue<int>  q;
    dist[s] = 0;
    q.push(s);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        int du = dist[u];
        for (long v : edges[u]) {
            if (dist[v] < 0) {
                dist[v] = du + 1;
                if (v == t) return dist[v];
                q.push(v);
            }
        }
        for (long v : reverseEdges[u]) {
            if (dist[v] < 0) {
                dist[v] = du + 1;
                if (v == t) return dist[v];
                q.push(v);
            }
        }
    }
    return -1;
}

ComponentInfo Graph::buildLargestWCCConst() const {
    dsuInit(numVertices);
    for (int u = 0; u < numVertices; ++u)
        for (int v : edges[u]) dsuUnion(u,v);
    for (int u = 0; u < numVertices; ++u)
        for (int v : reverseEdges[u]) dsuUnion(u,v);

    std::vector<int> root(numVertices);
    std::unordered_map<int,int> sz;
    sz.reserve(numVertices);

    for (int v = 0; v < numVertices; ++v){
        root[v] = dsuFind(v);
        ++sz[root[v]];
    }

    int bigRoot = std::max_element(sz.begin(), sz.end(),
                    [](auto &a,auto &b){return a.second<b.second;})->first;

    ComponentInfo info;
    info.size = sz[bigRoot];
    info.vertices.reserve(info.size);
    for (int v = 0; v < numVertices; ++v)
        if (root[v]==bigRoot) info.vertices.push_back(v);

    return info;
}

DiamP90 Graph::estimateDiameterDoubleSweep() const {
    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<int> vid(0, numVertices - 1);
    int r = vid(rng);

    std::vector<int> d(numVertices, -1);
    std::queue<int> q;
    d[r] = 0; q.push(r);
    int a = r;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (long v : edges[u]) {
            if (d[v] < 0) { d[v] = d[u] + 1; q.push(v); a = v; }
        }
        for (long v : reverseEdges[u]) {
            if (d[v] < 0) { d[v] = d[u] + 1; q.push(v); a = v; }
        }
    }

    std::fill(d.begin(), d.end(), -1);
    d[a] = 0; q.push(a);
    int diam = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (long v : edges[u]) {
            if (d[v] < 0) {
                d[v] = d[u] + 1;
                q.push(v);
                diam = std::max(diam, d[v]);
            }
        }
        for (long v : reverseEdges[u]) {
            if (d[v] < 0) {
                d[v] = d[u] + 1;
                q.push(v);
                diam = std::max(diam, d[v]);
            }
        }
    }
    return {diam, -1};
}

DiamP90 Graph::distanceStatsRandomPairs(int numPairs) const {
    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<int> vid(0, numVertices - 1);

    std::vector<int> distList;
    distList.reserve(numPairs);

    for (int i = 0; i < numPairs; ++i) {
        int s = vid(rng), t = vid(rng);
        while (t == s) t = vid(rng);
        int d = exactDistance(s, t);
        if (d > 0) distList.push_back(d);
    }
    if (distList.empty()) return {0, 0};

    std::sort(distList.begin(), distList.end());
    int diam = distList.back();
    int idx90 = std::min(int(distList.size()) - 1,
                         int(0.9 * distList.size()));
    int p90 = distList[idx90];
    return {diam, p90};
}

DiamP90 Graph::snowballDiameterAndP90(int sampleSize, int iterations) const {
    comp = buildLargestWCCConst();
    const auto& compV = comp.vertices;
    int compN = compV.size();
    if (compN == 0) return {0,0};

    int bestDiam = 0, sumP90 = 0;
    const int INIT_NEI = 2;

    for (int it = 0; it < iterations; ++it) {
        int init = compV[0];
        for (int u : compV) {
            int deg = edges[u].size() + reverseEdges[u].size();
            if (deg >= INIT_NEI) { init = u; break; }
        }

        std::vector<int> sample = {init};
        int need = INIT_NEI;
        for (long v : edges[init]) {
            if (!need--) { sample.push_back(v); break; }
        }
        for (long v : reverseEdges[init]) {
            if (need > 0) { sample.push_back(v); --need; }
        }

        std::unordered_set<int> inS(sample.begin(), sample.end());
        std::queue<int> q;
        for (int u : sample) q.push(u);

        while (!q.empty() && int(sample.size()) < sampleSize) {
            int u = q.front(); q.pop();
            for (long v : edges[u]) {
                if (inS.insert(v).second && int(sample.size())<sampleSize) {
                    sample.push_back(v); q.push(v);
                }
            }
            for (long v : reverseEdges[u]) {
                if (inS.insert(v).second && int(sample.size())<sampleSize) {
                    sample.push_back(v); q.push(v);
                }
            }
        }

        int nS = sample.size();
        if (nS < 2) { sumP90 += 0; continue; }

        std::vector<int> toLocal(numVertices, -1);
        for (int i = 0; i < nS; ++i) toLocal[sample[i]] = i;

        std::vector<std::vector<int>> sadj(nS);
        for (int i = 0; i < nS; ++i) {
            int u = sample[i];
            for (long v : edges[u]) if (toLocal[v]>=0) sadj[i].push_back(toLocal[v]);
            for (long v : reverseEdges[u]) if (toLocal[v]>=0) sadj[i].push_back(toLocal[v]);
            auto &nbr = sadj[i];
            std::sort(nbr.begin(), nbr.end());
            nbr.erase(std::unique(nbr.begin(), nbr.end()), nbr.end());
        }

        auto bfs_local = [&](int s){
            std::vector<int> d(nS,-1);
            std::queue<int> qq;
            d[s] = 0; qq.push(s);
            int far = s;
            while (!qq.empty()) {
                int u = qq.front(); qq.pop();
                for (int v : sadj[u]) {
                    if (d[v]<0) {
                        d[v] = d[u]+1;
                        far = v;
                        qq.push(v);
                    }
                }
            }
            return std::make_pair(far,d);
        };

        std::mt19937_64 rng(12345+it);
        int r0 = rng()%nS;
        auto [a,_da] = bfs_local(r0);
        auto [b,db ] = bfs_local(a);
        bestDiam = std::max(bestDiam, db[b]);

        std::vector<int> allD;
        allD.reserve(size_t(nS)*(nS-1)/2);
        for (int i = 0; i < nS; ++i) {
            auto [_f, di] = bfs_local(i);
            for (int j = i+1; j < nS; ++j) {
                if (di[j]>=0) allD.push_back(di[j]);
            }
        }
        if (!allD.empty()) {
            std::sort(allD.begin(), allD.end());
            int idx90 = std::round(allD.size()*0.9);
            if (idx90 >= (int)allD.size()) idx90 = allD.size()-1;
            sumP90 += allD[idx90];
        }
    }

    int avgP90 = std::round(double(sumP90)/iterations);
    return { bestDiam, avgP90 };
}
