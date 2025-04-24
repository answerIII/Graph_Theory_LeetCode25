class Solution {
public:
    int countComponents(const int n, const vector<vector<bool>>& g) {
        vector visited(n, false);
        int res = 0;
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                stack<int> s;
                s.push(i);
                visited[i] = true;
                while (!s.empty()) {
                    const int v = s.top();
                    s.pop();
                    for (int j = 0; j < n; ++j) {
                        if (g[v][j] && !visited[j]) {
                            visited[j] = true;
                            s.push(j);
                        }
                    }
                }
                res++;
            }
        }
        return res;
    }

    bool possibleBipartition(const int n, const vector<vector<int>>& dislikes) {
        vector g(n, vector(n, true));
        for (auto p : dislikes) {
            g[p[0] - 1][p[1] - 1] = false;
            g[p[1] - 1][p[0] - 1] = false;
        }
        if (countComponents(n, g) < 3) {
            return true;
        }
        return false;
    }
};