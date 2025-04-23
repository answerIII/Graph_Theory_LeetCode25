#include <vector>

class Solution {
public:
    std::vector<std::vector<int>> res;
    std::vector<int> way;
    int n;
    void dfs(std::vector<std::vector<int>>& graph, int v) {
        for (const int& p : graph[v]) {
            way.push_back(p);
            if (p == n - 1) {
                res.push_back(way);
            }
            else {
                dfs(graph, p);
            }
            way.pop_back();
        }
    }
    std::vector<std::vector<int>> allPathsSourceTarget(std::vector<std::vector<int>>& graph) {
        n = graph.size();
        way.push_back(0);
        dfs(graph, 0);
        return res;
    }
};
