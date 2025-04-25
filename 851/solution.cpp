#include<bits/stdc++.h>

using namespace std;

class Solution {
private:
    vector<vector<int>> _graph;
    vector<int> _ans;
    vector<int> *_quiet;

    void dfs(int v){
        if(_ans[v] != -1){
            return;
        }

        _ans[v] = v;
        for(auto el : _graph[v]){
            dfs(el);

            if ((*_quiet)[_ans[el]] < (*_quiet)[_ans[v]]) {
                _ans[v] = _ans[el];
            }
        }
    }

public:
    vector<int> loudAndRich(vector<vector<int>>& richer, vector<int>& quiet) {
        _graph.resize(quiet.size());
        _ans.resize(quiet.size());
        fill(_ans.begin(), _ans.end(), -1);
        _quiet = &quiet;
        for(auto &pair : richer){
            _graph[pair[1]].push_back(pair[0]);
        }

        for(int i = 0; i < quiet.size(); ++i){
            dfs(i);
        }

        return _ans;
    }
};

int main(){
    Solution sol;

    vector<vector<int>> v{{1,0},{2,1},{3,1},{3,7},{4,3},{5,3},{6,3}};
    vector<int> r{3,2,5,4,6,1,7,0};

    for(auto el : sol.loudAndRich(v, r)){
        cout << el << " ";
    }

    return 0;
}
