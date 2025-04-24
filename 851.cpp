class Solution {
public:
    int DFS(int u, std::vector<std::vector<int>>& adjacency, std::vector<bool>& visited, std::vector<int>& answer, std::vector<int>& quiet) {
        int currentMin = quiet[u];
        int currentMinNode = u;
        visited[u] = true;
        for (auto v : adjacency[u]) {
            if (visited[v]) {
                if (currentMin > quiet[answer[v]]) {
                    currentMin = quiet[answer[v]];
                    currentMinNode = answer[v];
                }
            }
            else {
                int minNodeV = DFS(v, adjacency, visited, answer, quiet);
                if (currentMin > quiet[minNodeV]) {
                    currentMin = quiet[minNodeV];
                    currentMinNode = minNodeV;
                }
            }
        }
        answer[u] = currentMinNode;
        return currentMinNode;
    }

    std::vector<int> loudAndRich(std::vector<std::vector<int>>& richer, std::vector<int>& quiet) {
        int n = quiet.size();
        std::vector<int> answer(n);
        std::vector<std::vector<int>> adjacency(n);
        for (int i = 0; i < richer.size(); ++i) {
            adjacency[richer[i][1]].push_back(richer[i][0]); // reverse edges
        }
        std::vector<bool> visited(n, false);
      // using topsort idea
        for (int u = 0; u < n; ++u) {
            if (!visited[u]) {
                DFS(u, adjacency, visited, answer, quiet);
            }
        }
        return answer;
    }
};
