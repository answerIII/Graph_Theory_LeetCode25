/*
You are given an array start where start = [startX, startY] represents your
initial position (startX, startY) in a 2D space. You are also given the array
target where target = [targetX, targetY] represents your target position
(targetX, targetY).

The cost of going from a position (x1, y1) to any other position in the space
(x2, y2) is |x2 - x1| + |y2 - y1|.

There are also some special roads. You are given a 2D array specialRoads where
specialRoads[i] = [x1i, y1i, x2i, y2i, costi] indicates that the ith special
road goes in one direction from (x1i, y1i) to (x2i, y2i) with a cost equal to
costi. You can use each special road any number of times.

Return the minimum cost required to go from (startX, startY) to (targetX,
targetY).

let n = special_roads.size()
Time complexity: O(n^2 * log(n))
Additional space complexity: O(n ^ 2)
Idea: use dijkstra algo where nodes are special_roads
distance = distance(road1.end, road2.begin) + road2.cost
*/

#include <vector>
#include <cmath>
#include <queue>

class Solution {
public:
    [[nodiscard]] static int minimumCost(
        const std::vector<int>& start, const std::vector<int>& target,
        const std::vector<std::vector<int>>& special_roads) {
        const int total_roads = static_cast<int>(special_roads.size());
        std::vector<int> min_dist;
        min_dist.reserve(total_roads);
        std::priority_queue<int_pair, std::vector<int_pair>,
                            std::greater<int_pair>>
            min_heap;
        for (int i = 0; i < total_roads; ++i) {
            int new_dist = distFromPointToRoad(start, special_roads[i]);
            min_dist.push_back(new_dist);
            min_heap.push(int_pair{new_dist, i});
        }
        return dijkstraAlg(special_roads, start, target, min_dist, min_heap,
                           total_roads);
    }

private:
    using int_pair = std::pair<int, int>;

    [[nodiscard]] static int pointsDist(
        const std::vector<int>& point1,
        const std::vector<int>& point2) noexcept {
        return std::abs(point1[0] - point2[0]) +
               std::abs(point1[1] - point2[1]);
    }

    [[nodiscard]] static int distFromPointToRoad(const std::vector<int>& point,
                                                 const std::vector<int>& road) {
        return std::abs(road[0] - point[0]) + std::abs(road[1] - point[1]) +
               road[4];
    }

    // Dijkstra return min path from start to target
    [[nodiscard]] static int dijkstraAlg(
        const std::vector<std::vector<int>>& special_roads,
        const std::vector<int>& start, const std::vector<int>& target,
        std::vector<int>& min_dist,
        std::priority_queue<int_pair, std::vector<int_pair>,
                            std::greater<int_pair>>& min_heap,
        const int& total_roads) {
        int answer = pointsDist(start, target);
        std::vector<bool> visited(total_roads, false);
        int unvisited_nodes = total_roads;
        while (unvisited_nodes > 0) {
            auto [dist, node_ind] = min_heap.top();
            min_heap.pop();
            if (dist > min_dist[node_ind]) {
                continue;
            }
            visited[node_ind] = true;
            --unvisited_nodes;
            answer = std::min(
                answer, dist +
                            std::abs(special_roads[node_ind][2] - target[0]) +
                            std::abs(special_roads[node_ind][3] - target[1]));
            for (int i = 0; i < total_roads; ++i) {
                if (!visited[i]) {
                    int dist_to_road = std::abs(special_roads[i][0] -
                                                special_roads[node_ind][2]) +
                                       std::abs(special_roads[i][1] -
                                                special_roads[node_ind][3]) +
                                       special_roads[i][4];
                    if (int new_dist = dist_to_road + dist;
                        new_dist < min_dist[i]) {
                        min_dist[i] = new_dist;
                        min_heap.push(int_pair{new_dist, i});
                    }
                }
            }
        }
        return answer;
    }
};