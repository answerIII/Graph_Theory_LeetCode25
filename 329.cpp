#include <vector>

class Solution {
public:
    int** memo;
    int m;
    int n;
    int dirs[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    int dfs(std::vector<std::vector<int>>& matrix, int i, int j) {
        if (memo[i][j] != 0) {
            return memo[i][j];
        }
        int path(1);
        for (auto& dir : dirs) {
            int x = i + dir[0];
            int y = j + dir[1];
            if (x >= 0 && x < m && y >= 0 && y < n && matrix[x][y] > matrix[i][j]) {
                path = std::max(path, dfs(matrix, x, y) + 1);
            }
        }
        memo[i][j] = path;
        return path;
    }
    int longestIncreasingPath(std::vector<std::vector<int>>& matrix) {
        m = matrix.size();
        n = matrix[0].size();
        int res = 0;
        memo = new int*[m];
        for (int i = 0; i < m; ++i) {
            memo[i] = new int[n];
            for (int j = 0; j < n; ++j) {
                memo[i][j] = 0;
            }
        }
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                res = std::max(res, dfs(matrix, i, j));
            }
        }
        for (int i = 0; i < m; ++i) {
            delete[] memo[i];
        }
        delete[] memo;
        return res;
    }
};
