class Solution {
public:

    static constexpr int ALPH_SIZE = 26;

    int values[ALPH_SIZE]{};
    bool vars[ALPH_SIZE]{ false };
    vector<pair<int, bool>> g[ALPH_SIZE];
    int max_value = -1;

    bool dfs(const int v, const int prev, bool equal) {
        if (values[v] != -1) {
            return !(equal && values[v] != values[prev] || !equal && values[v] == values[prev]);
        }
        if (v == prev) { // значит только начали обход
            max_value = -1;
        }
        if (equal) {
            values[v] = values[prev];
        } else {
            values[v] = ++max_value;
        }
        return std::ranges::all_of(g[v], [v, this](const auto& u) {
            return dfs(u.first, v, u.second);
        });
    }
    bool equationsPossible(vector<string>& equations) {
        for (auto& x : values) {
            x = -1;
        }
        for (const auto& s : equations) {
            if (s[0] == s[3]) {
                if (s[1] == '=') {
                    continue;
                }
                return false;
            }
            g[s[0] - 'a'].emplace_back(s[3] - 'a', s[1] == '=');
            g[s[3] - 'a'].emplace_back(s[0] - 'a', s[1] == '=');
            vars[s[0] - 'a'] = true;
            vars[s[3] - 'a'] = true;
        }
        for (int v = 0; v < ALPH_SIZE; ++v) {
            if (vars[0] && values[v] == -1 && !dfs(v, v, false)) {
                return false;
            }
        }
        return true;
    }
};