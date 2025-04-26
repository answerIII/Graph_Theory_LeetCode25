class Solution {
public:

    vector<vector<pair<int, bool>>> g; // true - прямой, false - обратный путь
    vector<int> visited;
    int res = 0;

    void dfs(const int v) {
        visited[v] = true;
        for (const auto [u, forward] : g[v]) {
            if (!visited[u]) {
                if (forward) {
                    ++res;
                }
                dfs(u);
            }
        }
    }
    int minReorder(const int n, const vector<vector<int>>& connections) {
        g.resize(n);
        visited.resize(n);
        for (const auto& e : connections) {
            g[e[0]].emplace_back(e[1], true);
            g[e[1]].emplace_back(e[0], false);
        }
        dfs(0);
        return res;
    }
};