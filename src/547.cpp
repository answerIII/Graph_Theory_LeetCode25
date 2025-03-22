// class Solution {
// public:

//     void dfs(std::vector<std::vector<int>>& isConnected, std::vector<bool>& visited, int i){
//         visited[i] = true;
//         for(int j = 0; j < isConnected.size() ; ++j){
//             if(isConnected[i][j] == 1 and !visited[j]){
//                 dfs(isConnected, visited, j);
//             }
//         }
//     }
//     int findCircleNum(std::vector<std::vector<int>>& isConnected) {
//         int n = isConnected.size();
//         std::vector<bool> visited(n, false);
//         int provinces = 0;

//         for(int i=0; i < n; ++i){
//             if(!visited[i]){
//                 dfs(isConnected, visited, i);
//                 ++provinces;
//             }
//         }
//         return provinces;
//     }
// };

#include <vector>

class Solution {
public:

    int get_parent(std::vector<int>& parent, int i){
        if(parent[i] == i){
            return i;
        }
        return parent[i] = get_parent(parent, parent[i]);
    }

    void Union_set(std::vector<int>& parent, int i, int j){
        int parent_i = get_parent(parent, i);
        int parent_j = get_parent(parent, j);

        if(parent_i != parent_j){
            parent[j] = parent_i;
        }
    }

    int findCircleNum(std::vector<std::vector<int>>& isConnected) {
        int n = isConnected.size();
        std::vector<int> parent(n);
        for(int i = 0; i < n; ++i){
            parent[i] = i;
        }

        for(int i = 0; i < n; ++i){
            for(int j = i+1; j < n; ++j){
                if(isConnected[i][j] == 1){
                    Union_set(parent, i, j);
                }
            }
        }

        int provinces = 0;
        for(int i = 0; i < n; ++i){
            if(parent[i] == i){
                ++provinces;
            }
        }
        return provinces;
    }
};

