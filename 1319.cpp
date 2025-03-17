class Solution {
    vector<int> parent;      
public:
    int makeConnected(int n, vector<vector<int>>& connections) {
        if (n - 1 > connections.size()){
            return -1;
        }
        parent.resize(n);
        for(int i = 0; i < n; ++i){ parent[i] = i; }
	    int count = -1;

	    for (int i = 0; i < connections.size(); ++i) {
            if (find(connections[i][0]) != find(connections[i][1])) {
                unite(connections[i][0], connections[i][1]);
            }
        }

        for (int i = 0; i < n; ++i) {
            if (i == parent[i]) {
                count++;
            }
        }
        return count;
    }

    int find(int x){
        if(parent[x] == x) { return x; }
        return parent[x] = find(parent[x]);
    }

    void unite(int x, int y){
        parent[find(x)] = find(y);
    }   
};
