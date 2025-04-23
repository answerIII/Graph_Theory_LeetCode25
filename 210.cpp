#include <vector>

class Solution {
public:
    int current_label;
    bool* visited;
    std::vector<int> res;
    void dfs(std::vector<std::vector<int>>& prerequisites, int v) {
        visited[v] = true;
        for (auto p : prerequisites) {
            if (p[1] == v && !visited[p[0]]) {
                dfs(prerequisites, p[0]);
            }
            else if (p[1] == v && res[p[0]] == NULL) {
                current_label = -2;
                return;
            }
        }
        if (current_label == -2) {
            return;
        }
        res[current_label] = v;
        --current_label;
    }
    std::vector<int> findOrder(int numCourses, std::vector<std::vector<int>>& prerequisites) {
        visited = new bool[numCourses];
        current_label = numCourses - 1;
        res.resize(numCourses);
        res[0] = -1;
        for (int i = 0; i < numCourses; ++i) {
            visited[i] = false;
        }
        for (int i = 0; i < numCourses; ++i) {
            if (!visited[i]) {
                dfs(prerequisites, i);
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
