#include <vector>

class Solution {
public:
    std::vector<std::vector<int>> res;
    int n;
    void dfs(std::vector<std::vector<int>>& graph, int v, std::vector<int> way) {
        for (auto p : graph[v]) {
            std::vector<int> nway = way;
            nway.push_back(p);
            if (p == n - 1) {
                res.push_back(nway);
            }
            else {
                dfs(graph, p, nway);
            }
        }
    }
    std::vector<std::vector<int>> allPathsSourceTarget(std::vector<std::vector<int>>& graph) {
        n = graph.size();
        std::vector<int> way;
        way.push_back(0);
        dfs(graph, 0, way);
        return res;
    }
};
