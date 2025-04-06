/*
You are given a 0-indexed 2D integer array pairs where pairs[i] = [starti,
endi]. An arrangement of pairs is valid if for every index i where 1 <= i <
pairs.length, we have endi-1 == starti.

Return any valid arrangement of pairs.
Note : The inputs will be generated such that there exists a valid arrangement
           of pairs.
Time complexity: O(E)
Additional space complexity: O(E)
Idea: https://cp-algorithms.com/graph/euler_path.html
*/

#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm>

class Solution {
public:
    [[nodiscard]] static std::vector<std::vector<int>> validArrangement(
        const std::vector<std::vector<int>>& pairs) {
        std::unordered_map<int, std::vector<int>> adj_list;
        std::unordered_map<int, int> vertex_power;
        fillData(pairs, adj_list, vertex_power);
        int start_vertex = pairs[0][0];
        for (const auto& [vertex, power] : vertex_power) {
            if (power == 1) {
                start_vertex = vertex;
                break;
            }
        }
        std::vector<int> euler_path;
        std::stack<int> vertex_stack;
        vertex_stack.push(start_vertex);
        while (!vertex_stack.empty()) {
            const int& new_vertex = vertex_stack.top();
            std::vector<int>& adjacent_vertices = adj_list[new_vertex];
            if (adjacent_vertices.size() == 0) {
                euler_path.push_back(new_vertex);
                vertex_stack.pop();
            } else {
                vertex_stack.push(adjacent_vertices.back());
                adjacent_vertices.pop_back();
            }
        }
        std::reverse(euler_path.begin(), euler_path.end());
        std::vector<std::vector<int>> ans;
        const int euler_path_size = static_cast<int>(euler_path.size());
        ans.reserve(euler_path.size() - 1);
        for (int i = 0; i < euler_path_size - 1; ++i) {
            ans.push_back(std::vector<int>{euler_path[i], euler_path[i + 1]});
        }
        return ans;
    }

    static void fillData(const std::vector<std::vector<int>>& pairs,
                         std::unordered_map<int, std::vector<int>>& adj_list,
                         std::unordered_map<int, int>& vertex_power) {
        for (const auto& edge : pairs) {
            adj_list[edge[0]].push_back(edge[1]);
            ++vertex_power[edge[0]];
            --vertex_power[edge[1]];
        }
    }
};
