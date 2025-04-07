/*
There are a total of numCourses courses you have to take, labeled from 0 to
numCourses - 1. You are given an array prerequisites where prerequisites[i] =
[ai, bi] indicates that you must take course ai first if you want to take course
bi.

    For example, the pair [0, 1] indicates that you have to take course 0 before
you can take course 1.

Prerequisites can also be indirect. If course a is a prerequisite of course b,
and course b is a prerequisite of course c, then course a is a prerequisite of
course c.

You are also given an array queries where queries[j] = [uj, vj]. For the jth
query, you should answer whether course uj is a prerequisite of course vj or
not.

Return a boolean array answer, where answer[j] is the answer to the jth query.
Time complexity: O(n ^ 3 + q) where n = num_courses, q = queries.size()
Additional space complexity: O(kMaxCourses * n + prerequisites.size())
Idea: use vector of bitset to represent info of all prerequisites,
if prerequisite(a, b) than bitset(a) |= bitset(b)
use topological sort to add undirected prerequisites in right order
*/

#include <vector>
#include <bitset>
#include <unordered_map>

class Solution {
public:
    static std::vector<bool> checkIfPrerequisite(
        int num_courses, const std::vector<std::vector<int>>& prerequisites,
        const std::vector<std::vector<int>>& queries) {
        std::unordered_map<int, std::vector<int>> adj_list;
        std::vector<std::bitset<kMaxCourses>> graph_reachability(num_courses);
        for (const auto& arc : prerequisites) {
            graph_reachability[arc[0]].set(arc[1]);
            adj_list[arc[0]].push_back(arc[1]);
        }
        std::vector<int> nodes_order;
        topSort(adj_list, nodes_order, num_courses);
        for (const int& node : nodes_order) {
            for (int i = 0; i < num_courses; ++i) {
                if ((graph_reachability[node] >> i &
                     std::bitset<kMaxCourses>(1))
                        .any()) {
                    graph_reachability[node] |= graph_reachability[i];
                }
            }
        }
        std::size_t queries_size = queries.size();
        std::vector<bool> answer(queries_size, false);
        for (std::size_t i = 0; i < queries_size; ++i) {
            answer[i] = ((graph_reachability[queries[i][0]] >> queries[i][1]) &
                         std::bitset<kMaxCourses>(1))
                            .any();
        }
        return answer;
    }

private:
    static void dfs(std::unordered_map<int, std::vector<int>>& adj_list,
                    std::vector<bool>& visited, std::vector<int>& nodes_order,
                    int node_ind) {
        visited[node_ind] = true;
        for (const int& adj_node : adj_list[node_ind]) {
            if (!visited[adj_node]) {
                dfs(adj_list, visited, nodes_order, adj_node);
            }
        }
        nodes_order.push_back(node_ind);
    }

    static void topSort(std::unordered_map<int, std::vector<int>>& adj_list,
                        std::vector<int>& nodes_order, const int total_nodes) {
        std::vector<bool> visited(total_nodes, false);
        for (int i = 0; i < total_nodes; ++i) {
            if (!visited[i]) {
                dfs(adj_list, visited, nodes_order, i);
            }
        }
    }

    static constexpr std::size_t kMaxCourses = 100;
};
