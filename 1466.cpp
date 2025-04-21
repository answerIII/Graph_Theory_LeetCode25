class Solution {
public:
    void DFS(int u, int prev, std::vector<std::vector<int>>& adjacency, int& res) {
        for (auto v : adjacency[u]) {
            if (abs(v) != prev) {
                res += (v > 0);
                DFS(abs(v), u, adjacency, res);
            }
        }
    }

    int minReorder(int n, std::vector<std::vector<int>>& connections) {
        int res = 0;
        std::vector<std::vector<int>> adjacency(n);
        for (int i = 0; i < connections.size(); ++i) {
            adjacency[connections[i][0]].push_back(connections[i][1]);
            adjacency[connections[i][1]].push_back(-connections[i][0]);
        }
        DFS(0, -1, adjacency, res);
        return res;
    }
};
