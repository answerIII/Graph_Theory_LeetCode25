/*
You are given a directed graph of n nodes numbered from 0 to n - 1, where each
node has at most one outgoing edge.

The graph is represented with a given 0-indexed array edges of size n,
indicating that there is a directed edge from node i to node edges[i]. If there
is no outgoing edge from i, then edges[i] == -1.

You are also given two integers node1 and node2.

Return the index of the node that can be reached from both node1 and node2, such
that the maximum between the distance from node1 to that node, and from node2 to
that node is minimized. If there are multiple answers, return the node with the
smallest index, and if no possible answer exists, return -1.

Note that edges may contain cycles.
Time complexity: O(V)
Additional memory complexity: O(V)
Idea: launch dfs(node1), dfs(node2), saving all visited nodes and distance from
staring node
*/

#include <vector>
#include <limits>

class Solution {
public:
    [[nodiscard]] static int closestMeetingNode(const std::vector<int>& edges,
                                                int node1, int node2) {
        const int total_nodes = static_cast<int>(edges.size());
        std::vector<bool> visited1(total_nodes, false);
        std::vector<int> distance1(total_nodes, 0);
        std::vector<bool> visited2(total_nodes, false);
        std::vector<int> distance2(total_nodes, 0);
        dfs(edges, visited1, distance1, node1);
        dfs(edges, visited2, distance2, node2);
        int answer = -1;
        int min_dist = std::numeric_limits<int>::max();
        for (int i = 0; i < total_nodes; ++i) {
            if (visited1[i] && visited2[i] &&
                min_dist > std::max(distance1[i], distance2[i])) {
                answer = i;
                min_dist = std::max(distance1[i], distance2[i]);
            }
        }
        return answer;
    }

private:
    static void dfs(const std::vector<int>& edges, std::vector<bool>& visited,
                    std::vector<int>& distance, int start_node) noexcept {
        int curr_dist = 0;
        int curr_node = start_node;
        while (curr_node != -1 && !visited[curr_node]) {
            distance[curr_node] = curr_dist++;
            visited[curr_node] = true;
            curr_node = edges[curr_node];
        }
    }
};