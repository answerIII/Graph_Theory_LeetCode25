/*
There is an undirected graph with n nodes numbered from 0 to n - 1 (inclusive).
You are given a 0-indexed integer array values where values[i] is the value of
the ith node. You are also given a 0-indexed 2D integer array edges, where each
edges[j] = [uj, vj, timej] indicates that there is an undirected edge between
the nodes uj and vj, and it takes timej seconds to travel between the two nodes.
Finally, you are given an integer maxTime.

A valid path in the graph is any path that starts at node 0, ends at node 0, and
takes at most maxTime seconds to complete. You may visit the same node multiple
times. The quality of a valid path is the sum of the values of the unique nodes
visited in the path (each node's value is added at most once to the sum).

Return the maximum quality of a valid path.

Note: There are at most four edges connected to each node.
Time complexity: We will go through all possible paths of the graph. Since
max_path_length = 10 (max_time = 100, min_edge_time = 10) and each node has at
most 4 adjacent_nodes, than O(adjacent_nodes ^ max_path_length)
Additional space complexity: O(V + E + max_path_length)
Idea: use dfs-like approach to visit all nodes from current one
*/

#include <vector>

class Solution {
public:
    [[nodiscard]] static int maximalPathQuality(
        const std::vector<int>& values,
        const std::vector<std::vector<int>>& edges, const int& max_time) {
        const int total_nodes = static_cast<int>(values.size());
        std::vector<std::vector<Destination>> graph(total_nodes);
        for (const auto& edge : edges) {
            graph[edge[0]].push_back(
                Destination{.node_ = edge[1], .time_ = edge[2]});
            graph[edge[1]].push_back(
                Destination{.node_ = edge[0], .time_ = edge[2]});
        }
        std::vector<int> node_visits(total_nodes, 0);
        node_visits[0] = 1;
        return getMaxQuality(values, graph, node_visits, 0, values[0], 0,
                             max_time);
    }

private:
    struct Destination {
        int node_;
        int time_;
    };

    [[nodiscard]] static int getMaxQuality(
        const std::vector<int>& values,
        std::vector<std::vector<Destination>>& graph,
        std::vector<int>& node_visits, const int& start_node,
        const int& curr_quality, const int& curr_time,
        const int& max_time) noexcept {
        int max_quality = (start_node == 0) ? curr_quality : 0;
        for (const Destination& dest : graph[start_node]) {
            if (int new_time = curr_time + dest.time_; new_time <= max_time) {
                int new_quality = curr_quality;
                if (node_visits[dest.node_] == 0) {
                    new_quality += values[dest.node_];
                }
                ++node_visits[dest.node_];
                max_quality = std::max(
                    max_quality,
                    getMaxQuality(values, graph, node_visits, dest.node_,
                                  new_quality, new_time, max_time));
                --node_visits[dest.node_];
            }
        }
        return max_quality;
    }
};