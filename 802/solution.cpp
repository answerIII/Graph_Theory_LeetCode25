#include<bits/stdc++.h>

using namespace std;

class Solution {
private:
    vector<char> _visited;
    vector<vector<int>> *_graph;


    bool dfs(int v){
        if(_visited[v]){
            return _visited[v] == 2;
        }

        _visited[v] = 1;
        for(auto el : (*_graph)[v]){
            if(!dfs(el)){
                return 0;
            }
        }

        _visited[v] = 2;
        return 1;
    }

public:
    vector<int> eventualSafeNodes(vector<vector<int>>& graph) {
        _visited.assign(graph.size(), 0);
        vector<int> ans;
        _graph = &graph;

        for(int i = 0; i < graph.size(); ++i){
            if(dfs(i)){
                ans.push_back(i);
            }
        }

        return ans;
    }
};

int main(){
    Solution sol;

    vector<vector<int>> v{{1,2},{2,3},{5},{0},{5},{},{}};

    for(auto el : sol.eventualSafeNodes(v)){
        cout << el << " ";
    }

    return 0;
}
