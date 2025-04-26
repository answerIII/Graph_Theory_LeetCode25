#include <vector>
class Solution {
public:
    int longestCycle(std::vector<int>& edges) {
        int n = edges.size();
        int max_cycle = -1;

        for (int i = 0; i < n; ++i) {
            std::vector<int> visited(n, -1); // visited[u] = шаг, когда мы попали в вершину
            int curr = i;
            int time = 0;

            while (curr != -1) {
                if (visited[curr] != -1) {
                    // Найден цикл: длина = текущее время - шаг, когда впервые зашли в curr
                    max_cycle = std::max(max_cycle, time - visited[curr]);
                    break;
                }
                visited[curr] = time++;
                curr = edges[curr];
            }
        }

        return max_cycle;
    }
};
