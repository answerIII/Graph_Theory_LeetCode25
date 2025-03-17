class Solution {
private:
    void dfs(vector<bool>& visited, const vector<vector<int>>& edges, vector<bool>& forbidden, int vertex) {
        visited[vertex] = true;
        for (int target : edges[vertex]) {
            if (!forbidden[target] && !visited[target]) {
                dfs(visited, edges, forbidden, target);
            }
        }
    }
int count;
public:
    int reachableNodes(int n, vector<vector<int>>& edges, vector<int>& restricted) {
        vector<bool> visited(n, false);
        vector<vector<int>> graph(n);
        for (vector<int>& edge : edges) {
            graph[edge[0]].push_back(edge[1]);
            graph[edge[1]].push_back(edge[0]);
        }
        vector<bool> forbidden(n);
        for (int x : restricted) {
            forbidden[x] = true;
        }
        dfs(visited, graph, forbidden, 0);
        int ans = 0;
        for (int i = 0; i < n; ++i) {
            ans += visited[i];
        }
        return ans;
    }
};