class Solution {
    int dist[200][200];    
    int width;
    int height;
public:
    int longestIncreasingPath(vector<vector<int>>& matrix) {
        height = matrix.size();
        width = matrix[0].size();
        int n = height * width;
        memset(dist, -1, sizeof(dist));

        int maximum = 0;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                maximum = max(maximum, countDepth(i, j, matrix));
            }
        }
        return maximum;
    }

    int countDepth(int i, int j, vector<vector<int>>& matrix) {
        if (dist[i][j] != -1) { return dist[i][j]; }

        int value = 1;
        if (i + 1 < height && matrix[i + 1][j] > matrix[i][j]) {
            value = max(value, countDepth(i + 1, j, matrix) + 1);
        } 
        if (i - 1 > -1 && matrix[i - 1][j] > matrix[i][j]) {
            value = max(value, countDepth(i - 1, j, matrix) + 1);
        } 
        if (j + 1 < width && matrix[i][j + 1] > matrix[i][j]) {
            value = max(value, countDepth(i, j + 1, matrix) + 1);
        } 
        if (j - 1 > -1 && matrix[i][j - 1] > matrix[i][j]) {
            value = max(value, countDepth(i, j - 1, matrix) + 1);
        }
        
        dist[i][j] = value;
        return dist[i][j];
    }

};
