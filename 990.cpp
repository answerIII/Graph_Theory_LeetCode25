class Solution {
public:

    static constexpr int ALPH_SIZE = 26;

    vector<int> g[ALPH_SIZE]{};
    bool visited[ALPH_SIZE]{};

    void dfs(const int v, vector<int>& component) {
        if (visited[v]) {
            return;
        }
        visited[v] = true;
        component.push_back(v);
        for (const auto& u : g[v]) {
            dfs(u, component);
        }
    }
    bool equationsPossible(const vector<string>& equations) {
        int v_components[ALPH_SIZE]{};
        vector<int> eq_with_not;
        for (int i = 0; i < equations.size(); ++i) {
            if (equations[i][1] == '!') {
                if (equations[i][0] == equations[i][3]) {
                    return false;
                }
                eq_with_not.push_back(i);
                g[equations[i][0] - 'a'].push_back(equations[i][0] - 'a');
                g[equations[i][3] - 'a'].push_back(equations[i][3] - 'a');
            }
            else {
                g[equations[i][0] - 'a'].push_back(equations[i][3] - 'a');
                g[equations[i][3] - 'a'].push_back(equations[i][0] - 'a');
            }
        }
        vector<int> component;
        int count_of_component = 1;
        component.reserve(ALPH_SIZE);
        for (int i = 0; i < ALPH_SIZE; ++i) {
            if (g[i].empty()) {
                continue;
            }
            dfs(i, component);
            for (const auto& v : component) {
                v_components[v] = count_of_component;
            }
            if (!component.empty()) {
                ++count_of_component;
            }
            component.resize(0);
        }
        return ranges::all_of(eq_with_not, [&v_components, &equations](auto i) {
            return v_components[equations[i][0] - 'a'] != v_components[equations[i][3] - 'a'];
        });
    }
};