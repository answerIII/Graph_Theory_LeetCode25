#include <vector>
#include <queue>
#include <tuple>


class Solution {
public:
    bool findSafeWalk(vector<vector<int>>& grid, int health) {
        int total_cols = grid[0].size();
        int total_rows = grid.size();

        std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        std::queue<std::tuple<int, int, int>> q; // row, col, health
        std::vector<std::tuple<int, int, int>> visited;

        q.push({0,0, health});
        visited.push_back({0, 0, health});

        while(!q.empty()){
            int x = std::get<0>(q.front());
            int y = std::get<1>(q.front());
            int h = std::get<2>(q.front());

            q.pop();

            if(x == total_rows-1 && y == total_cols-1 && h >= 1){
                return true;
            }

            for(int i = 0; i < directions.size(); ++i){
                int nx = x + directions[i].first;
                int ny = y + directions[i].second;

                if(nx >= 0 && nx < total_rows && ny >= 0 && ny < total_cols){
                    int new_health = h - grid[nx][ny];

                    // если здоровье остается положительным и клетка не была посещена
                    bool is_visited = false;
                    for (int j = 0; j < visited.size(); ++j) {
                        if (std::get<0>(visited[j]) == nx && std::get<1>(visited[j]) == ny && std::get<2>(visited[j]) == new_health) {
                            is_visited = true;
                            break;
                        }
                    }

                    if (new_health > 0 && !is_visited) {
                        q.push(std::make_tuple(nx, ny, new_health)); 
                        visited.push_back({nx, ny, new_health}); 
                    }
                }
            }
        }

        return false; 
    }
};
