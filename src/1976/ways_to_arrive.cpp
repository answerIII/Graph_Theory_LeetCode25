/*
You are in a city that consists of n intersections numbered from 0 to n - 1 with
bi-directional roads between some intersections. The inputs are generated such
that you can reach any intersection from any other intersection and that there
is at most one road between any two intersections.

You are given an integer n and a 2D integer array roads where roads[i] = [ui,
vi, timei] means that there is a road between intersections ui and vi that takes
timei minutes to travel. You want to know in how many ways you can travel from
intersection 0 to intersection n - 1 in the shortest amount of time.

Return the number of ways you can arrive at your destination in the shortest
amount of time. Since the answer may be large, return it modulo 10^9 + 7.
Time complexity: O(E * log(V))
Additional space complexity: O(V + E)
Idea: use dijkstra algorithm to find shortest and number of paths simultaneously
*/

#include <vector>
#include <queue>
#include <cstdint>
#include <limits>

class Solution {
public:
    [[nodiscard]] static int countPaths(
        int n, const std::vector<std::vector<int>>& roads) {
        std::vector<uint64_t> min_dist(n, std::numeric_limits<uint64_t>::max());
        std::vector<int> ways(n, 0);
        std::priority_queue<std::pair<uint64_t, int>,
                            std::vector<std::pair<uint64_t, int>>,
                            std::greater<std::pair<uint64_t, int>>>
            min_heap;
        std::vector<std::vector<std::pair<int, int>>> adj_matrix(n);
        min_dist[0] = 0;
        ways[0] = 1;
        min_heap.emplace(0, 0);
        for (const auto& road : roads) {
            uint64_t road_len = static_cast<uint64_t>(road[2]);
            adj_matrix[road[0]].emplace_back(road[1], road_len);
            adj_matrix[road[1]].emplace_back(road[0], road_len);
        }
        dijkstra(adj_matrix, min_dist, ways, min_heap);
        return static_cast<int>(ways.back());
    }

private:
    static constexpr int kMOD = 1E9 + 7;

    static void dijkstra(
        const std::vector<std::vector<std::pair<int, int>>>& adj_list,
        std::vector<uint64_t>& min_dist, std::vector<int>& ways,
        std::priority_queue<std::pair<uint64_t, int>,
                            std::vector<std::pair<uint64_t, int>>,
                            std::greater<std::pair<uint64_t, int>>>& min_heap) {
        while (!min_heap.empty()) {
            auto [dist, node_ind] = min_heap.top();
            min_heap.pop();
            if (min_dist[node_ind] < dist) {
                continue;
            }
            for (const auto& [neighbour_id, dist_to_neighbour] :
                 adj_list[node_ind]) {
                uint64_t new_dist = dist + dist_to_neighbour;
                uint64_t prev_dist = min_dist[neighbour_id];
                if (prev_dist > new_dist) {
                    min_dist[neighbour_id] = new_dist;
                    ways[neighbour_id] = ways[node_ind];
                    min_heap.emplace(new_dist, neighbour_id);
                } else if (min_dist[neighbour_id] == new_dist) {
                    ways[neighbour_id] =
                        (ways[neighbour_id] + ways[node_ind]) % kMOD;
                }
            }
        }
    }
};