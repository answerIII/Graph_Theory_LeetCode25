class Solution {
    vector<int> parent;
public:
    bool equationsPossible(vector<string>& equations) {
        parent.resize(26);
        for(int i = 0; i < 26; ++i){ parent[i] = i; }
	  
	    for (int i = 0; i < equations.size(); ++i) {
            if (equations[i][1] == '=') {
                unite(equations[i][0] - 'a', equations[i][3] - 'a');
            }
        }

        for (int i = 0; i < equations.size(); ++i) {
            if (equations[i][1] == '!') {
                if (find(equations[i][0] - 'a') == find(equations[i][3] - 'a')) return false;
            }
        }

        return true;
    }

    int find(int x){
        if(parent[x] == x) { return x; }
        return parent[x] = find(parent[x]);
    }

    void unite(int x, int y){
        parent[find(x)] = find(y);
    }  
};
