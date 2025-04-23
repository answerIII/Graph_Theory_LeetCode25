#include <vector>

class Solution {
public:
    int current_label;
    char* visited;
    std::vector<int> res;
    void dfs(std::vector<std::vector<int>>& adj, int v) {
        visited[v] = 1;
        for (auto p : adj[v]) {
            if (visited[p] == 0) {
                dfs(adj, p);
            }
            else if (visited[p] == 1) {
                current_label = -2;
                return;
            }
        }
        if (current_label == -2) {
            return;
        }
        res[current_label] = v;
        visited[v] = 2;
        --current_label;
    }
    std::vector<int> findOrder(int numCourses, std::vector<std::vector<int>>& prerequisites) {
        std::vector<std::vector<int>> adj(numCourses);
        for (const auto& edge : prerequisites) {
            adj[edge[1]].push_back(edge[0]);
        }
        visited = new char[numCourses];
        current_label = numCourses - 1;
        res.resize(numCourses);
        for (int i = 0; i < numCourses; ++i) {
            visited[i] = 0;
        }
        for (int i = 0; i < numCourses; ++i) {
            if (visited[i] == 0) {
                dfs(adj, i);
                if (current_label == -2) {
                    break;
                }
            }
        }
        delete[] visited;
        if (current_label == -2) {
            return {};
        }
        return res;
    }
};
