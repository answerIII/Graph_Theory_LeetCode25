#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>

class Solution {
public:
    void dfs(int node, std::unordered_map<int, std::deque<int>>& graph, std::vector<std::vector<int>>& result) {
        while (!graph[node].empty()) {
            int neighbour = graph[node].front();
            graph[node].pop_front();
            dfs(neighbour, graph, result);
            result.push_back({ node, neighbour });
        }
    }

    std::vector<std::vector<int>> validArrangement(std::vector<std::vector<int>>& pairs) {
        std::unordered_map<int, std::deque<int>> graph;
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
        dfs(start_node, graph, result);
        std::reverse(result.begin(), result.end());
        return result;
    }
};
