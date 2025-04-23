class Solution {
public:
    // Ford-Bellman algo
    int findCheapestPrice(int n, std::vector<std::vector<int>>& flights, int src, int dst, int k) {
        std::vector<std::vector<int>> d(k + 2, std::vector<int>(n, INT_MAX));
        d[0][src] = 0;
        int u;
        int v;
        for (int i = 1; i <= k + 1; ++i) {
            for (int j = 0; j < flights.size(); ++j) {
                u = flights[j][0];
                v = flights[j][1];
                if (d[i - 1][u] != INT_MAX && d[i][v] > d[i - 1][u] + flights[j][2]) {
                    d[i][v] = d[i - 1][u] + flights[j][2];
                }
            }
        }
        return d[k + 1][dst] != INT_MAX ? d[k + 1][dst] : -1;
    }
};
