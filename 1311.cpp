#include <ranges>

class Solution {
public:
    vector<string> watchedVideosByFriends(const vector<vector<string>>& watchedVideos,
            const vector<vector<int>>& friends, const int id, const int level) {
        int n = friends.size();
        queue<int> q;
        q.push(id);
        vector<int> d(n, -1);
        vector<bool> visited(n, false);
        d[id] = 0;
        for (int i = 1; ; ++i) {
            // добавить проверку что тут уже были 
            int v = q.front();
            q.pop();
            visited[v] = true;
            if (i == level) {
                unordered_map<string, int> watched_count;
                for (const int u : friends[v]) {
                    if (visited[u]) continue;
                    for (const string& s : watchedVideos[u]) {
                        ++watched_count[s];
                    }
                }
                vector<pair<string, int>> tmp(
                    make_move_iterator(watched_count.begin()),
                    make_move_iterator(watched_count.end())
                );
                watched_count.clear();
                ranges::sort(tmp);
                vector<string> res;
                res.reserve(tmp.size());
                for (auto& str : tmp | views::keys) {
                    res.emplace_back(std::move(str));
                }
                return res;
            }
            for (int u : friends[v]) {
                if (visited[u]) continue;
                if (d[u] == -1) {
                    q.push(u);
                    d[u] = d[v] + 1;
                }
            }
        }
    }
};