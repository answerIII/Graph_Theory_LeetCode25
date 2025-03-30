class Solution {
    private:
        void dijkstra(int start, int n, vector<vector<int>>& graph, vector<int>& shortest_path, vector<vector<int>>& edges) {
            priority_queue<pair<int,int>, vector<pair<int,int>>, std::less<>> vertex_queue;
            vertex_queue.push({start, 0});
            int vertex;
            while (!vertex_queue.empty()) {
                auto[vertex, dist] = vertex_queue.top();
                vertex_queue.pop();
                if (dist > shortest_path[vertex]) continue;
                for (int edge : graph[vertex]) {
                    int target = edges[edge][0] != vertex ? edges[edge][0] : edges[edge][1];
                    int weight = edges[edge][2];
                    if (shortest_path[vertex] + weight < shortest_path[target]) {
                        shortest_path[target] = shortest_path[vertex] + weight;
                        vertex_queue.push({target, shortest_path[target]});
                    }
                }
            }
    
        }
    public:
        vector<bool> findAnswer(int n, vector<vector<int>>& edges) {
            vector<vector<int>> graph(n);
            int edges_count = edges.size();
            for (int i = 0; i < edges_count; ++i) {
                graph[edges[i][0]].push_back(i);
                graph[edges[i][1]].push_back(i);
            }
            vector<int> shortest_path(n, INT_MAX);
            vector<bool> result(edges_count, false);
            vector<bool> visited(n, false);
    
            queue<int> bfs_queue;
            bfs_queue.push(0);
            shortest_path[0] = 0;
            dijkstra(0, n, graph, shortest_path, edges);

            if (shortest_path[n-1] == INT_MAX) {
                return result;
            }
    
            for (int i = 0; i < n; ++i) {
                visited[i] = false;
            }
            visited[0] = true;
            bfs_queue.push(n-1);
            while (!bfs_queue.empty()) {
                int u = bfs_queue.front();
                bfs_queue.pop();
                if (visited[u]) continue;
                visited[u] = true;
    
                for (int edge : graph[u]) {
                    int v;
                    if (edges[edge][0] != u) v = edges[edge][0];
                    else v = edges[edge][1];
                    if (shortest_path[u] - edges[edge][2] != shortest_path[v]) {
                        continue;
                    }
                    result[edge] = true;
                    bfs_queue.push(v);
                }
            }
     
            return result;
    
        }
    };