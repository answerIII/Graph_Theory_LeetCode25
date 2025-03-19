class Solution {
    public:
        int minReorder(int n, vector<vector<int>>& connections) {
            
            vector<vector<int>> edge_in(n);
            vector<vector<int>> edge_out(n);
            
            for (vector<int>& edge : connections) {
                edge_in[edge[1]].push_back(edge[0]);
                edge_out[edge[0]].push_back(edge[1]);
            }
    
            vector<bool> visited(n, false);
            queue<int> bfs_q;
            bfs_q.push(0);
            visited[0] = true;
    
            int curr_vertex;
            int result = 0;
            while (!bfs_q.empty()) {
                curr_vertex = bfs_q.front();
                bfs_q.pop();
                for (int v : edge_in[curr_vertex]) {
                    if (visited[v]) continue;
                    bfs_q.push(v);
                }
                for (int v : edge_out[curr_vertex]) {
                    if (visited[v]) continue;
                    ++result;
                    bfs_q.push(v);
                }
                visited[curr_vertex] = true;
            }
    
            return result;
    
        }
    };