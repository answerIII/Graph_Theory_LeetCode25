class Solution {
public:
    void DFS(int u, std::vector<std::vector<int>>& graph, std::vector<int>& currentPath, std::vector<std::vector<int>>& allPaths) {
        currentPath.push_back(u);
        if (u == graph.size() - 1) {
            allPaths.push_back(currentPath);
            currentPath.pop_back();
            return;
        }
        for (auto v : graph[u]) {
            DFS(v, graph, currentPath, allPaths);
        }
        currentPath.pop_back();
    }

    std::vector<std::vector<int>> allPathsSourceTarget(std::vector<std::vector<int>>& graph) {
        std::vector<int> currentPath;
        std::vector<std::vector<int>> allPaths;
        DFS(0, graph, currentPath, allPaths);
        return allPaths;
    }
};
