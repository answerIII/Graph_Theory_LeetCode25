#include<bits/stdc++.h>

using namespace std;

class Solution {
    const int INF = INT_MAX;
    using ll = long long;
    vector<vector<int>> *_edges;
    vector<vector<ll>> _graph;
    vector<ll> _dist;
    vector<bool> _visited;
    int _n;
    int _src;

    void dp(){
        for(auto &el : _graph) {
            fill(el.begin(), el.end(), INF);
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

        for (int i = 0; i < _n; ++i) {
            int last = -1;
            for (int j = 0; j < _n; ++j) {
                if (!_visited[j] && (last == -1 || _dist[j] < _dist[last])) {
                    last = j;
                    break;
                }
            }

            _visited[last] = true;
            for (int j = 0; j < _n; ++j) {
                _dist[j] = min(_dist[j], _dist[last] + _graph[last][j]);
            }
        }
    }

public:
    vector<vector<int>> modifiedGraphEdges(int n, vector<vector<int>>& edges, int source, int destination, int target) {
        _edges = &edges;
        _graph.resize(n);
        fill(_graph.begin(), _graph.end(), vector<ll>(n));
        _dist.resize(n);
        _visited.resize(n);
        _n = n;
        _src = source;
        
        dp();

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
            dp();
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

    vector<vector<int>> v{{4,1,-1},{2,0,-1},{0,3,-1},{4,3,-1}};

    for(auto x : sol.modifiedGraphEdges(5, v, 0, 1, 5)){
        cout << x[0] << " " << x[1] << " " << x[2] << endl;
    }

    return 0;
}
