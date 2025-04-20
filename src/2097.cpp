#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>
#include <stack>

class Solution {
public:
 std::vector<std::vector<int>> validArrangement(std::vector<std::vector<int>>& pairs) {
        std::unordered_map<int, std::vector<int>> graph;
        std::unordered_map<int, int> in_degree, out_degree;

        for (int i = 0; i < pairs.size(); ++i) {
            int start = pairs[i][0];
            int end = pairs[i][1];
            graph[start].push_back(end);
            out_degree[start]++;
            in_degree[end]++;
        }

        int start_node = pairs[0][0]; 
        for (auto& entry : graph) {
            int node = entry.first;
            if (out_degree[node] > in_degree[node]) {
                start_node = node;
                break;
            }
        }

        std::vector<std::vector<int>> result;
        std::stack<int> stk;
        stk.push(start_node);

        while (!stk.empty()) {
            int node = stk.top();
            if (!graph[node].empty()) {
                int neighbour = graph[node].back();
                graph[node].pop_back();
                stk.push(neighbour);
            }
            else {
                stk.pop();
                if(!stk.empty()) {
                    result.push_back({stk.top(), node});
                }
            }
        }

        std::reverse(result.begin(), result.end());
        return result;
    }
};

