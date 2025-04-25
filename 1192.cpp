class Solution {
public:

    vector<vector<int>> g;
    vector<int> time;
    vector<int> low;
    vector<vector<int>> bridges;
    int timer = 0;

    void dfs(const int u, const int parent = -1) {
        if (time[u] != -1) {
            return;
        }
        time[u] = low[u] = timer++;
        for (int v : g[u]) {
            if (v == parent) {
                continue;
            }
            if (time[u] == -1) {
                low[u] = min(low[u], time[v]);
            }
            else {
                dfs(v, u);
                low[u] = min(low[u], low[v]);
                if (low[v] > time[u]) {
                    bridges.push_back({ u, v });
                }
            }
        }
    }
    vector<vector<int>> criticalConnections(const int n, const vector<vector<int>>& connections) {
        g.resize(n);
        for (const auto& e : connections) {
            g[e[0]].push_back(e[1]);
            g[e[1]].push_back(e[0]);
        }
        time.assign(n, -1);
        low.assign(n, -1);
        for (int i = 0; i < n; ++i) {
            if (time[i] == -1) {
                dfs(i);
            }
        }
        return bridges;
    }
};