#include <vector>

class Solution {
public:
    int current_label;
    int now;
    char* visited;
    std::vector<int> res;
    void dfs(std::vector<std::vector<int>>& prerequisites, int v) {
        visited[v] = 1;
        for (auto p : prerequisites) {
            if (p[1] == v && visited[p[0]] == 0) {
                dfs(prerequisites, p[0]);
            }
            else if (p[1] == v && visited[p[0]] == 1) {
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
        visited = new char[numCourses];
        current_label = numCourses - 1;
        res.resize(numCourses);
        for (int i = 0; i < numCourses; ++i) {
            visited[i] = 0;
            res[i] = -1;
        }
        for (int i = 0; i < numCourses; ++i) {
            if (!visited[i]) {
                now = i;
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
