class Solution {
public:

    int leader(std::vector<int>& p, int v) {
        return (v == p[v]) ? v : p[v] = leader(p, p[v]);
    }
    void unite(std::vector<int>& p, std::vector<int>& s, int a, int b) {
        a = leader(p, a);
        b = leader(p, b);
        if (a == b) {
            return;
        }
        if (s[a] > s[b]) {
            std::swap(a, b);
        }
        s[b] += s[a];
        p[a] = b;
    }

    long long countPairs(int n, std::vector<std::vector<int>>& edges) {
        std::vector<int> p(n);
        std::vector<int> s(n);
        for (int i = 0; i < n; ++i) {
            p[i] = i;
            s[i] = 1;
        }
        for (int i = 0; i < edges.size(); ++i) {
            unite(p, s, edges[i][0], edges[i][1]);
        }
        std::unordered_set<int> components;
        size_t result = static_cast<size_t>(n) * (n - 1) / 2;
        for (int i = 0; i < n; ++i) {
            auto it = components.insert(leader(p, i));
            if (it.second) {
                result -= static_cast<size_t>(s[*it.first]) * (s[*it.first] - 1) / 2;
            }
        }
        return result;
    }
};
