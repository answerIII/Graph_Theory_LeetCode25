class Solution {
public:

    void DFS(int u, std::vector<std::vector<int>>& adjacency, std::vector<bool>& visited, size_t& k) {
        ++k;
        visited[u] = true;
        for (auto v : adjacency[u]) {
            if (!visited[v]) {
                DFS(v, adjacency, visited, k);
            }
        }
    }

    long long countPairs(int n, std::vector<std::vector<int>>& edges) {
        std::vector<std::vector<int>> adjacency(n);
        for (int i = 0; i < edges.size(); ++i) {
            adjacency[edges[i][0]].push_back(edges[i][1]);
            adjacency[edges[i][1]].push_back(edges[i][0]);
        }
        std::vector<bool> visited(n, false);
        size_t result = static_cast<size_t>(n) * (n - 1) / 2;
        size_t k;
        for (int i = 0; i < n; ++i) {
            k = 0;
            if (!visited[i]) {
                DFS(i, adjacency, visited, k);
            }
            result -= k * (k - 1) / 2;
        }
        return result;
    }
};
