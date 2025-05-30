#include "Graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <cstdio>
#include <thread>
#include <mutex>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <atomic>
#include <random>

Graph::Graph()
    : numVertices(0)
    , numEdges(0)
    , isLargeGraph(false)
    , maxDegreeVertex(-1)
{}

void Graph::loadFromFile(const std::string& path, const std::string& format) {
    numVertices = numEdges = 0;
    isLargeGraph = false;
    maxDegreeVertex = -1;
    edges.clear();
    reverseEdges.clear();

    FILE* file = fopen(path.c_str(), "rb");
    if (!file) throw std::runtime_error("Cannot open file: " + path);

    constexpr long long BUFFER_SIZE = 1024 * 1024 * 1024; // 1GB
    std::vector<char> buffer(BUFFER_SIZE);
    size_t bytesRead = fread(buffer.data(), 1, BUFFER_SIZE, file);
    fclose(file);
    if (bytesRead == 0) return;

    int skipFirst = (format == "csv") ? 1 : (format == "mtx") ? 2 : 0;
    char commentPrefix = (format == "txt") ? '#' : '%';

    std::vector<std::string_view> lines;
    lines.reserve(1000000);
    for (size_t start = 0, i = 0; i < bytesRead; ++i) {
        if (buffer[i] == '\n' || buffer[i] == '\r') {
            if (i > start) lines.emplace_back(&buffer[start], i - start);
            start = i + 1;
        }
    }
    if (!lines.empty() && lines.back().size() == 0) lines.pop_back();

    if (skipFirst > 0 && lines.size() > static_cast<size_t>(skipFirst)) {
        lines.erase(lines.begin(), lines.begin() + skipFirst);
    }

    size_t nThreads = std::thread::hardware_concurrency();
    if (nThreads == 0) nThreads = 4;

    std::unordered_set<int> vertexSet;
    std::mutex vertexMutex;

    std::vector<std::unordered_map<int,std::vector<int>>> localEdges(nThreads);
    std::vector<std::unordered_map<int,std::vector<int>>> localReverseEdges(nThreads);

    auto worker = [&](size_t tid, size_t begin, size_t end) {
        auto& locE = localEdges[tid];
        auto& locR = localReverseEdges[tid];

        for (size_t i = begin; i < end; ++i) {
            auto& line = lines[i];
            if (line.empty() || line[0] == commentPrefix) continue;

            const char* ptr = line.data();
            char* endPtr;
            int u = std::strtol(ptr, &endPtr, 10);
            if (endPtr == ptr) continue;
            while (*endPtr==' '||*endPtr==','||*endPtr=='\t') ++endPtr;
            char* endPtr2;
            int v = std::strtol(endPtr, &endPtr2, 10); 
            if (endPtr2==endPtr) continue;
            if (u<0||v<0) continue;

            {
                std::lock_guard<std::mutex> lk(vertexMutex);
                vertexSet.insert(u);
                vertexSet.insert(v);
            }
            locE[u].push_back(v);
            locR[v].push_back(u);
        }
    };

    std::vector<std::thread> threads;
    size_t blockSize = (lines.size() + nThreads - 1) / nThreads;
    for (size_t t = 0; t < nThreads; ++t) {
        size_t begin = t * blockSize;
        size_t end = std::min(lines.size(), begin + blockSize);
        threads.emplace_back(worker, t, begin, end);
    }
    for (auto& th : threads) th.join();

    std::vector<int> vertList(vertexSet.begin(), vertexSet.end());
    std::sort(vertList.begin(), vertList.end());
    numVertices = vertList.size();

    std::unordered_map<int,int> idToIndex;
    idToIndex.reserve(numVertices);
    for (int i = 0; i < static_cast<int>(numVertices); ++i) {
        idToIndex[vertList[i]] = i;
    }

    edges.assign(numVertices, {});
    reverseEdges.assign(numVertices, {});

    numEdges = 0;
    for (size_t t = 0; t < nThreads; ++t) {
        for (auto& kv : localEdges[t]) {
            int u0 = kv.first;
            int u = idToIndex[u0];
            for (int v0 : kv.second) {
                int v = idToIndex[v0];
                edges[u].push_back(v);
                reverseEdges[v].push_back(u);
                ++numEdges;
            }
        }
    }

    degrees.resize(numVertices, 0);
    for (int u = 0; u < numVertices; ++u) {
        std::unordered_set<int> neigh(edges[u].begin(), edges[u].end());
        neigh.insert(reverseEdges[u].begin(), reverseEdges[u].end());
        degrees[u] = neigh.size();
    }

    dStats.minDeg = *std::min_element(degrees.begin(), degrees.end());
    dStats.maxDeg = *std::max_element(degrees.begin(), degrees.end());

    long long sumDeg = 0;
    for (int k : degrees) sumDeg += k;
    dStats.avgDeg = numVertices ? double(sumDeg) / numVertices : 0.0;

    degHist.assign(dStats.maxDeg + 1, 0);
    for (int k : degrees) ++degHist[k];

    maxDegreeVertex = -1;
    int maxDeg = -1;
    for (int u = 0; u < static_cast<int>(numVertices); ++u) {
        int deg = edges[u].size() + reverseEdges[u].size();
        if (deg > maxDeg) {
            maxDeg = deg;
            maxDegreeVertex = u;
        }
    }

    const size_t LARGE_THRESHOLD = 10000; 
    isLargeGraph = (numEdges > LARGE_THRESHOLD);
    isDirected = (path.find("directed") != std::string::npos && path.find("undirected") == std::string::npos);
}

int Graph::getVertexCount() const {
    return numVertices;
}

int Graph::getEdgeCount() const {
    return numEdges;
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

int Graph::countWeaklyConnectedComponents() {
    return isLargeGraph ? countWeaklyConnectedComponentsDSU() : countWeaklyConnectedComponentsBFS();
}

double Graph::getDensity() const {
    if (numVertices <= 1) return 0.0;

    double maxEdges = isDirected
        ? static_cast<double>(numVertices) * (numVertices - 1)
        : static_cast<double>(numVertices) * (numVertices - 1) / 2;

    return static_cast<double>(numEdges) / maxEdges;
}

void Graph::dfs1(int u, std::vector<bool>& vis, std::vector<int>& ord) {
    vis[u] = true;
    for (int v : edges[u])
        if (!vis[v]) dfs1(v, vis, ord);
    ord.push_back(u);
}
void Graph::dfs2(int u, std::vector<bool>& vis, int& sz) {
    vis[u] = true;
    sz++;
    for (int v : reverseEdges[u])
        if (!vis[v]) dfs2(v, vis, sz);
}

int Graph::countStronglyConnectedComponents() {
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

Graph::ComponentInfo Graph::buildLargestWCC() {
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

    wccRatio = double(info.size)/numVertices;
    return info;
}

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

Graph::Ordering Graph::degeneracyOrder(const std::vector<std::vector<int>>& adj) const {
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

double Graph::averageClusteringLargestWCC(){
    ComponentInfo comp = buildLargestWCC();
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

Graph::DegreeStats Graph::getDegreeStats() const {
    return dStats;
}

const std::vector<int>& Graph::degreeHistogram() const {
    return degHist;
}

static int largestWCC_ignore(const std::vector<std::vector<long int>>& E, const std::vector<std::vector<long int>>& R, const std::vector<char>& removed)
{
    const int n = removed.size();
    std::vector<int> parent(n), rnk(n,0);
    auto Find=[&](auto&& self,int x)->int{
        return parent[x]==x ? x : parent[x]=self(self,parent[x]);
    };
    auto Union=[&](int a,int b){
        a=Find(Find,a); b=Find(Find,b);
        if(a==b) return;
        if(rnk[a]<rnk[b]) parent[a]=b;
        else if(rnk[a]>rnk[b]) parent[b]=a;
        else { parent[b]=a; ++rnk[a]; }
    };

    for(int i=0;i<n;++i) parent[i]=i;

    for(int u=0;u<n;++u) if(!removed[u]){
        for(int v:E[u]) if(!removed[v]) Union(u,v);
        for(int v:R[u]) if(!removed[v]) Union(u,v);
    }

    std::unordered_map<int,int> sz;
    sz.reserve(n);
    for(int v=0;v<n;++v) if(!removed[v])
        ++sz[Find(Find,v)];

    int mx = 0;
    for(auto &kv:sz) if(kv.second>mx) mx=kv.second;
    return mx;
}

double Graph::ratioAfterRemoval(double perc, bool targetedByDeg, uint32_t seed) const
{
    if(perc<=0.0) return wccRatio;
    if(perc>=100.0) return 0.0;

    int k = std::lround(numVertices * perc / 100.0);
    if(k==0) k=1;

    std::vector<int> victims;
    victims.reserve(k);

    if(targetedByDeg){
        std::vector<int> idx(numVertices);
        std::iota(idx.begin(),idx.end(),0);
        std::partial_sort(idx.begin(), idx.begin()+k, idx.end(),
                          [&](int a,int b){ return degrees[a]>degrees[b]; });
        victims.assign(idx.begin(), idx.begin()+k);
    }else{
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> d(0, numVertices-1);
        std::unordered_set<int> vis;
        while(int(vis.size())<k) vis.insert(d(rng));
        victims.assign(vis.begin(), vis.end());
    }

    std::vector<char> removed(numVertices,0);
    for(int v: victims) removed[v]=1;

    int lcc = largestWCC_ignore(edges, reverseEdges, removed);
    int left = numVertices - k;

    return left ? double(lcc) / left : 0.0;
}

void Graph::precomputeLandmarks(int k, LandmarkSelect sel, uint32_t seed)
{
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

int Graph::estimateDiameterDoubleSweep() const {
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
    return diam;
}

std::pair<int,double> Graph::distanceStatsRandomPairs(int numPairs) const {
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
    if (distList.empty()) return {0, 0.0};

    std::sort(distList.begin(), distList.end());
    int diam = distList.back();
    int idx90 = std::min(int(distList.size()) - 1,
                         int(0.9 * distList.size()));
    double p90 = distList[idx90];
    return {diam, p90};
}

std::vector<int> Graph::snowballSample(int targetSize) const {
    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<int> vid(0, numVertices - 1);

    std::vector<int> sample;
    sample.reserve(targetSize);
    std::vector<char> used(numVertices, 0);
    std::queue<int> q;

    for (int i = 0; i < 3; ++i) {
        int u = vid(rng);
        if (!used[u]) {
            used[u] = 1;
            sample.push_back(u);
            q.push(u);
        }
    }

    while (!q.empty() && int(sample.size()) < targetSize) {
        int u = q.front(); q.pop();
        for (long v : edges[u]) {
            if (!used[v] && int(sample.size()) < targetSize) {
                used[v] = 1;
                sample.push_back(v);
                q.push(v);
            }
        }
        for (long v : reverseEdges[u]) {
            if (!used[v] && int(sample.size()) < targetSize) {
                used[v] = 1;
                sample.push_back(v);
                q.push(v);
            }
        }
    }
    return sample;
}

std::pair<int,double> Graph::distanceStatsOnSample(const std::vector<int>& sample, int numPairs) const
{
    int nS = sample.size();
    if (nS == 0) return {0, 0.0};

    std::vector<int> toLocal(numVertices, -1);
    for (int i = 0; i < nS; ++i) toLocal[sample[i]] = i;

    std::vector<std::vector<int>> sadj(nS);
    for (int i = 0; i < nS; ++i) {
        int u = sample[i];
        for (long v : edges[u])
            if (toLocal[v] >= 0) sadj[i].push_back(toLocal[v]);
        for (long v : reverseEdges[u])
            if (toLocal[v] >= 0) sadj[i].push_back(toLocal[v]);
        auto& nbr = sadj[i];
        std::sort(nbr.begin(), nbr.end());
        nbr.erase(std::unique(nbr.begin(), nbr.end()), nbr.end());
    }

    auto bfs_local = [&](int start){
        std::vector<int> d(nS, -1);
        std::queue<int> q;
        d[start] = 0; q.push(start);
        int far = start;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : sadj[u]) {
                if (d[v] < 0) {
                    d[v] = d[u] + 1;
                    far = v;
                    q.push(v);
                }
            }
        }
        return std::make_pair(far, d);
    };

    std::mt19937_64 rng(12345);
    int r0 = rng() % nS;
    auto [a,_] = bfs_local(r0);
    auto [b,d2] = bfs_local(a);
    int diam = d2[b];

    std::uniform_int_distribution<int> vidS(0, nS - 1);
    std::vector<int> distList;
    distList.reserve(numPairs);
    for (int i = 0; i < numPairs; ++i) {
        int s = vidS(rng), t = vidS(rng);
        while (t == s) t = vidS(rng);
        auto [_, dvec] = bfs_local(s);
        if (dvec[t] >= 0) distList.push_back(dvec[t]);
    }
    std::sort(distList.begin(), distList.end());
    double p90 = 0.0;
    if (!distList.empty()) {
        int idx90 = std::min(int(distList.size()) - 1,
                             int(0.9 * distList.size()));
        p90 = distList[idx90];
    }

    return {diam, p90};
}

Graph::ComponentInfo Graph::buildLargestWCCConst() const {
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

Graph::DiamP90 Graph::snowballDiameterAndP90(int sampleSize, int iterations) const {
    ComponentInfo comp = buildLargestWCCConst();
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