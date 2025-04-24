class Solution {
public:
    struct State {
        int time;
        int i;
        int j;
        bool is_one_to_move;
        bool operator<(const State& other) const {
            return this->time > other.time;
        }
    };

    int INF = INT32_MAX;

    int minTimeToReach(vector<vector<int>>& moveTime) {
        int n = moveTime.size();
        int m = moveTime[0].size();
        vector t(n, vector(m, INF));
        t[0][0] = 0;
        priority_queue<State> q;
        q.push({ moveTime[0][0], 0, 0, false });
        while (!q.empty()) {
            auto [time, i, j, is_one_to_move] = q.top();
            int cost = is_one_to_move ? 2 : 1;
            q.pop();
            if (i != 0 && j != 0 && time > t[i][j]) continue;
            for (pair<int, int> adj[4] = { {i - 1, j}, {i, j - 1}, {i + 1, j}, {i, j + 1} }; auto [x, y] : adj) {
                if (x < 0 || x >= n || y < 0 || y >= m) continue;
                if (t[x][y] > max(moveTime[x][y], t[i][j]) + cost) {
                    t[x][y] = max(moveTime[x][y], t[i][j]) + cost;
                    q.push({ t[x][y], x, y, !is_one_to_move });
                }
            }
        }
        return t[n - 1][m - 1];
    }
};