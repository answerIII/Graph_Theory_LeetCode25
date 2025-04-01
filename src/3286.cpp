#include <vector>
#include <queue>

class Solution {
public:
    bool findSafeWalk(std::vector<std::vector<int>>& grid, int health) {
        int total_rows = grid.size();
        int total_cols = grid[0].size();
        std::vector<std::vector<int>> dist(total_rows, std::vector<int>(total_cols, INT_MAX));
        
        dist[0][0] = grid[0][0];
        std::queue<std::pair<int, int>> q;
        q.emplace(0, 0);
        
        std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        
        while (!q.empty()) {
            int x = q.front().first;
            int y = q.front().second;
            q.pop();
            
            for (int i = 0; i < directions.size(); ++i) {
                int nx = x +  directions[i].first;;
                int ny = y + directions[i].second;
                
                
                if (nx >= 0 && nx < total_rows && ny >= 0 && ny < total_cols) {
                    int new_health = dist[x][y] + grid[nx][ny];
                    
                    if (new_health < dist[nx][ny]) {
                        dist[nx][ny] = new_health;
                        q.emplace(nx, ny);
                    }
                }
            }
        }
        
        bool ans = false;

        if(dist[total_rows - 1][total_cols - 1] < health){
            ans = true;
        }

        return ans;
    }
};

