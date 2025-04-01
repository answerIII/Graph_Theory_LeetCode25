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
        std::vector<std::pair<int, int>> visited;

        q.push({0,0, health});
        visited.push_back({0, 0});

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

                    if( new_health >= 1 && std::find(visited.begin(), visited.end(), std::make_pair(nx,ny)) == visited.end()){
                        q.push(std::make_tuple(nx, ny, new_health));
                        visited.push_back(std::make_pair(nx, ny));
                    }
                }
            }
        }
    return false;

    }
};
