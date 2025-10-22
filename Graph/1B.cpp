#include "Graph.h"

static int largestWCC_ignore(const std::vector<std::vector<long int>>& E, const std::vector<std::vector<long int>>& R, const std::vector<char>& removed) {
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

double Graph::ratioAfterRemoval(double perc, bool targetedByDeg, uint32_t seed) const {
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
    int left = numVertices;

    return left ? double(lcc) / left : 0.0;
}
