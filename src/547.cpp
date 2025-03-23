#include <vector>

class Solution {
public:

    void dfs(std::vector<std::vector<int>>& isConnected, std::vector<bool>& visited, int i){
        visited[i] = true;
        for(int j = 0; j < isConnected.size() ; ++j){
            if(isConnected[i][j] == 1 and !visited[j]){
                dfs(isConnected, visited, j);
            }
        }
    }
    int findCircleNum(std::vector<std::vector<int>>& isConnected) {
        int n = isConnected.size();
        std::vector<bool> visited(n, false);
        int provinces = 0;

        for(int i=0; i < n; ++i){
            if(!visited[i]){
                dfs(isConnected, visited, i);
                ++provinces;
            }
        }
        return provinces;
    }
};
