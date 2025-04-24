class Solution {
public:

    static constexpr int ALPH_SIZE = 26;

    int values[ALPH_SIZE]{};
    bool vars[ALPH_SIZE]{ false };
    int g[ALPH_SIZE][ALPH_SIZE]{}; // 0 - нет ребра, -1 - не равны, 1 - равны
    int max_value = -1;

    bool dfs(const int v, const int prev) {
        if (values[v] != -1) {
            return !(g[v][prev] == 1 && values[v] != values[prev] || g[v][prev] == -1 && values[v] == values[prev]);
        }
        if (v == prev) { // значит только начали обход
            max_value = -1;
        }
        if (g[v][prev] == 1) {
            values[v] = values[prev];
        } else {
            values[v] = ++max_value;
        }
        for (int i = 0; i < ALPH_SIZE; ++i) {
            if (g[i][v] != 0) {
                if (!dfs(i, v)) {
                    return false;
                }
            }
        }
        return true;
    }
    bool equationsPossible(vector<string>& equations) {
        for (auto& x : values) {
            x = -1;
        }
        for (const auto& s : equations) {
            int eq = s[1] == '=' ? 1 : -1;
            if (s[0] == s[3]) {
                if (eq == 1) { // a == a
                    continue;
                }
                return false; // a != a
            }
            if (g[s[0] - 'a'][s[3] - 'a'] == eq) { // уже добавляли это условие
                continue;
            }
            if (g[s[0] - 'a'][s[3] - 'a'] == -eq) { // условие противоречит уже добавленному
                return false;
            }
            g[s[0] - 'a'][s[3] - 'a'] = eq;
            g[s[3] - 'a'][s[0] - 'a'] = eq;
            vars[s[0] - 'a'] = true;
            vars[s[3] - 'a'] = true;
        }
        for (int v = 0; v < ALPH_SIZE; ++v) {
            if (vars[0] && values[v] == -1 && !dfs(v, v)) {
                return false;
            }
        }
        return true;
    }
};