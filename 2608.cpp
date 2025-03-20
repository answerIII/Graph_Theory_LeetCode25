class Solution {
    vector<vector<int>> adj;
    vector<int> depth;
    int len;
public:
    int findShortestCycle(int n, vector<vector<int>>& edges) {
        adj.resize(n);
       for (int i = 0; i < edges.size(); ++i) {
            adj[edges[i][0]].push_back(edges[i][1]);
            adj[edges[i][1]].push_back(edges[i][0]);
        }
        len = INT_MAX;
        depth.resize(n, INT_MAX);   
     
        for (int i = 0; i < n; ++i) {
            if (depth[i] == INT_MAX) {
                depth[i] = 0;
                dfs(i, 0);            
            }
        }
        
        if (len == INT_MAX) {
            return -1;
        }
        return len;
    }


    void dfs(int node, int prev) {     
        for (int neighbor : adj[node]) {     
            if (neighbor == prev) continue; 

            if (depth[neighbor] > depth[node] + 1){
                depth[neighbor] = depth[node] + 1;
                dfs(neighbor, node);
            }
            else if (depth[neighbor] < depth[node]) { 
                len = min(len, depth[node] - depth[neighbor] + 1);
            }
        }
    }
};
