class Solution {
public:
    int networkBecomesIdle(vector<vector<int>>& edges, vector<int>& patience) {
        
        int sz = patience.size();
        vector<vector<int>> adj(sz);
        for(int i = 0;i<edges.size();++i){
            adj[edges[i][0]].push_back(edges[i][1]);
            adj[edges[i][1]].push_back(edges[i][0]);
        }

        // for (int i = 0; i < adj.size(); ++i) {
        //     cout << "Node " << i << ": ";
        //     for (int j = 0; j<adj[i].size();++j) {
        //         cout << adj[i][j] << " ";
        //     }
        //     cout << "\n";
        // }

        vector<bool> visited(sz);
        vector<int> distance (sz);
        queue<int> q;
        
        for(int i = 0;i<sz;++i){
            distance[i] = -1;
        }
        q.push(0);
        distance[0] = 0;
        while(!q.empty()){
            int cur_node = q.front();
            int cur_distance = distance[cur_node];
            for(int i = 0;i<adj[cur_node].size();++i){
                if(distance[adj[cur_node][i]] == -1){
                    distance[adj[cur_node][i]] = cur_distance + 1;
                    q.push(adj[cur_node][i]);
                }
            }
            q.pop();
        }

        for(int i = 0;i<sz;++i){
            std::cout<<"dist "<< i << ":"<<" "<<distance[i]<<"\n";
        }
        return 0;
    }
};