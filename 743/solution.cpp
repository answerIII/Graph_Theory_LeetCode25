#include <algorithm>
#include<bits/stdc++.h>
#include <queue>

using namespace std;

class Solution {
private:
    const int INF = INT_MAX;

public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<vector<pair<int,int>>> g(n);
        for(auto &el : times){
            g[el[0]-1].push_back({el[1]-1, el[2]});
        }

        vector<int> dist(n, INF);
        dist[k-1]=0;

        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> q;
        q.push({0, k-1});

        while(!q.empty()){
            auto [d,u] = q.top(); q.pop();

            for(auto [v, w] : g[u]){
                if(dist[v] > dist[u] + w){
                    dist[v] = dist[u] + w;
                    q.push({dist[v], v});
                }
            }
        }

        int m = *max_element(dist.begin(), dist.end());
        if(m == INF){
            return -1;
        }
        return m;
    }
};

int main(){
    Solution sol;

    vector<vector<int>> v{{2,1,1},{2,3,1},{3,4,1}};

    cout << sol.networkDelayTime(v,4,2) << " ";

    return 0;
}
