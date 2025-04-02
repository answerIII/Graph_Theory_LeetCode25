/*
There are n rooms labeled from 0 to n - 1 and all the rooms are locked except
for room 0. Your goal is to visit all the rooms. However, you cannot enter a
locked room without having its key.

When you visit a room, you may find a set of distinct keys in it. Each key has a
number on it, denoting which room it unlocks, and you can take all of them with
you to unlock the other rooms.

Given an array rooms where rooms[i] is the set of keys that you can obtain if
you visited room i, return true if you can visit all the rooms, or false
otherwise.
Time complexity: O(total_keys + total_rooms)
Additional memory complexity: O(total_rooms)
Idea: use bfs to visit all rooms that can be visited
*/

#include <vector>
#include <queue>

class Solution {
public:
    [[nodiscard]] static bool canVisitAllRooms(
        const std::vector<std::vector<int>>& rooms) {
        return bfs(rooms) == static_cast<int>(rooms.size());
    }

private:
    // BFS returns number of visited nodes (rooms)
    [[nodiscard]] static int bfs(
        const std::vector<std::vector<int>>& rooms) noexcept {
        int visited_rooms = 0;
        std::queue<int> rooms_to_visit;
        std::vector<bool> visited(static_cast<int>(rooms.size()), false);
        visited[0] = true;
        rooms_to_visit.push(0);
        while (!rooms_to_visit.empty()) {
            int new_room = rooms_to_visit.front();
            rooms_to_visit.pop();
            ++visited_rooms;
            for (const auto& key : rooms[new_room]) {
                if (!visited[key]) {
                    rooms_to_visit.push(key);
                    visited[key] = true;
                }
            }
        }
        return visited_rooms;
    }
};