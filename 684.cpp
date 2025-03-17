class Solution {
    vector<int> parent;
public:
    vector<int> findRedundantConnection(vector<vector<int>>& edges) {
        parent.resize(edges.size() + 1);
        for(int i = 0; i < edges.size() + 1; ++i){ parent[i] = i; }
	    int count = -1;
        vector<int> edge(2);

	    for (int i = 0; i < edges.size(); ++i) {
            if (find(edges[i][0]) != find(edges[i][1])) {
                unite(edges[i][0], edges[i][1]);
            }
            else {
                edge[0] = edges[i][0];
                edge[1] = edges[i][1];
            }
        }

        return edge;
    }

    int find(int x){
        if(parent[x] == x) { return x; }
        return parent[x] = find(parent[x]);
    }

    void unite(int x, int y){
        parent[find(x)] = find(y);
    }  
};
