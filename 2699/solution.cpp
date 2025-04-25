#include<bits/stdc++.h>
#include <queue>

using namespace std;

class Solution {
    int INF;
    using ll = long long;
    vector<vector<int>> *_edges;
    vector<vector<ll>> _graph;
    vector<ll> _dist;
    vector<bool> _visited;
    int _n;
    int _src;

    void dijkstra(){
        for(auto &el : _graph) {
            fill(el.begin(), el.end(), -1);
        }
        fill(_dist.begin(), _dist.end(), INF);
        fill(_visited.begin(), _visited.end(), 0);
        _dist[_src] = 0;
        
        for(auto &edge : (*_edges)){
            if(edge[2] == -1){
                continue;
            }
            _graph[edge[0]][edge[1]] = _graph[edge[1]][edge[0]] = edge[2];
        }

        priority_queue<pair<int, int>, vector<pair<int,int>>, greater<>> q;
        q.push({0, _src});

        while(!q.empty()){
            auto [d,u] = q.top(); q.pop();

            for(int v = 0; v < _n; ++v){
                if(_graph[u][v] == -1){
                    continue;
                }
                if(_dist[v] > _dist[u] + _graph[u][v]){
                    _dist[v] = _dist[u] + _graph[u][v];
                    q.push({_dist[v], v});
                }
            }
        }
    }

public:
    vector<vector<int>> modifiedGraphEdges(int n, vector<vector<int>>& edges, int source, int destination, int target) {
        INF = target+1;
        _edges = &edges;
        _graph.resize(n);
        fill(_graph.begin(), _graph.end(), vector<ll>(n));
        _dist.resize(n);
        _visited.resize(n);
        _n = n;
        _src = source;
        
        dijkstra();

        if(_dist[destination] < target){
            return {};
        }

        bool flag = _dist[destination] == target;
        for(auto &edge : edges){
            if(edge[2] > 0){
                continue;
            }
            if(flag){
                edge[2] = INF;
                continue;
            }

            edge[2] = 1;
            dijkstra();
            if(_dist[destination] <= target){
                flag = true;
                edge[2] += target - _dist[destination];
            }
        }

        if(flag){
            return edges;
        }
        return {};
    }
};

int main(){
    Solution sol;

    vector<vector<int>> v{{1,3,10},{4,2,-1},{0,3,7},{4,0,7},{3,2,-1},{1,4,5},{2,0,8},{1,0,3},{1,2,5}};

    for(auto x : sol.modifiedGraphEdges(5, v, 3, 4, 11)){
        cout << x[0] << " " << x[1] << " " << x[2] << endl;
    }

    return 0;
}
