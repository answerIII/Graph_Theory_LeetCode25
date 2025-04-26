#include <vector>
#include <unordered_map>

class Solution {
public:
    int longestCycle(std::vector<int>& edges) {
        int n = edges.size();
        std::vector<bool> visited(n, false);
        int max_cycle = -1;

        for (int i = 0; i < n; ++i) {
            if (visited[i]) {
                continue;
            }

            std::unordered_map<int, int> nodeTime; // узел, время входа
            int time = 0;
            int curr = i;

            while (curr != -1 && nodeTime.find(curr) == nodeTime.end()) {
                if (visited[curr]) 
                    break;
                nodeTime[curr] = time++;
                visited[curr] = true;
                curr = edges[curr];
            }

 
            if (curr != -1 && nodeTime.find(curr) != nodeTime.end()) {
                int cycleLen = time - nodeTime[curr];
                max_cycle = std::max(max_cycle, cycleLen);
            }
        }

        return max_cycle;
    }
};
