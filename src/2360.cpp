#include <vector>

class Solution {
public:
    int longestCycle(std::vector<int>& edges) {
        int n = edges.size();
        std::vector<int> visited(n, 0);  // 0 = не посещён, 1 = в процессе, 2 = обработан
        std::vector<int> depth(n, 0);
        int max_cycle = -1;

        // Запускаем DFS для каждой вершины
        for (int i = 0; i < n; ++i) {
            if (visited[i] == 0) {
                dfs(i, edges, visited, depth,0, max_cycle);
            }
        }

        return max_cycle;
    }

private:
    void dfs(int node, std::vector<int>& edges, std::vector<int>& visited, std::vector<int>& depth, int cur_depth, int& max_cycle) {
        visited[node] = 1;   // помечаем, что в процессе посещения
        depth[node] = cur_depth; // сохраняем глубину входа

        int next = edges[node];
        if (next != -1) {
            if (visited[next] == 0) {
                // продолжаем обход
                dfs(next, edges, visited, depth, cur_depth + 1, max_cycle);
            }
            else if (visited[next] == 1) {
                // найден цикл
                max_cycle = std::max(max_cycle, cur_depth - depth[next] + 1);
            }
        }

        visited[node] = 2;  // помечаем как обработанный
    }
};
