class Solution {
public:
    bool possibleBipartition(const int n, const vector<vector<int>>& dislikes) {
        vector<vector<int>> g(n);
        for (const auto& u : dislikes) {
            g[u[0] - 1].push_back(u[1] - 1);
            g[u[1] - 1].push_back(u[0] - 1);
        }
        vector color(n, -1);
        for (int i = 0; i < n; ++i) {
            if (color[i] == -1) {
                queue<int> q;
                q.push(i);
                color[i] = 0;
                while (!q.empty()) {
                    int v = q.front();
                    q.pop();
                    for (int u : g[v]) {
                        if (color[u] == -1) {
                            color[u] = 1 - color[v];
                            q.push(u);
                        }
                        else if (color[u] == color[v]) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
};