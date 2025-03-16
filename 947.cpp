class Solution {
public:
    int removeStones(vector<vector<int>>& stones) {
        unordered_map<int, vector<int>> adj;
         
        int n = stones.size();

        vector<int> indices(n);
        for (int i = 0; i < n; i++) indices[i] = i;

        sort(indices.begin(), indices.end(), [&](int a, int b) {
            return stones[a][0] < stones[b][0];
        });

        for (int i = 1; i < n; i++) {
            int index = indices[i];
            if (stones[index][0] == stones[indices[i - 1]][0]) {
                adj[index].push_back(indices[i - 1]);
                adj[indices[i - 1]].push_back(index);
            }
        }

        sort(indices.begin(), indices.end(), [&](int a, int b) {
            return stones[a][1] < stones[b][1];
        });

        for (int i = 1; i < n; i++) {
            int index = indices[i];
            if (stones[index][1] == stones[indices[i - 1]][1]) {
                adj[index].push_back(indices[i - 1]);
                adj[indices[i - 1]].push_back(index);
            }
        }

        vector<int> visited(stones.size(), 0); 
        int count = 0;

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                dfs(i, adj, visited);
                count++;
            }
        }

        return stones.size() - count;
    }

    void dfs(int node, unordered_map<int, vector<int>>& adj, vector<int>& visited) {
        if (visited[node] == 1) return;  
      
        visited[node] = 1;
        for (int neighbor : adj[node]) {
            dfs(neighbor, adj, visited);
        }
    }    
};
