class Solution {
public:
    int networkBecomesIdle(vector<vector<int>>& edges, vector<int>& patience) {
        
        int sz = patience.size();
        vector<vector<int>> adj(sz);
        for(int i = 0;i<edges.size();++i){
            adj[edges[i][0]].push_back(edges[i][1]);
            adj[edges[i][1]].push_back(edges[i][0]);
        }

        for (int i = 0; i < adj.size(); ++i) {
            cout << "Node " << i << ": ";
            for (int j = 0; j<adj[i].size();++j) {
                cout << adj[i][j] << " ";
            }
            cout << "\n";
        }

        vector<bool> visited(sz);
        vector<int> distance (sz);
        queue<int> q;

        return 0;
    }
};
