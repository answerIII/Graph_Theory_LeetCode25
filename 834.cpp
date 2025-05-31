class Solution {
public:
    int calc_size(std::vector<int>& num_of_nodes, const vector<vector<int>>& adj, int node, int parent_node){
        if(adj[node].size() == 1 && node!=0){
            num_of_nodes[node] = 1;
            return 1;
        }
        else{
            int num = 0;
            for(int i = 0;i<adj[node].size();++i){
                if(adj[node][i] != parent_node){
                    num+=calc_size(num_of_nodes, adj, adj[node][i], node);
                }              
            }
            num_of_nodes[node] = num + 1;
            return num + 1;  
        }
    }
    vector<int> sumOfDistancesInTree(int n, vector<vector<int>>& edges) {
        
        std::vector<int> num_of_nodes(n);
        vector<vector<int>> adj(n);
        for(int i = 0;i<edges.size();++i){
            adj[edges[i][0]].push_back(edges[i][1]);
            adj[edges[i][1]].push_back(edges[i][0]);
        }

        calc_size(num_of_nodes, adj, 0, -1);
        vector<int> dist(n);
        queue<int> q;
        q.push(0);
        for(int i = 0;i<n;++i){
            dist[i] = -1;
        }
        dist[0] = 0;
        while(!q.empty()){
            int cur_node = q.front();
            int cur_distance = dist[cur_node];
            for(int i = 0;i<adj[cur_node].size();++i){
                if(dist[adj[cur_node][i]] == -1){
                    dist[adj[cur_node][i]] = cur_distance + 1;
                    q.push(adj[cur_node][i]);
                }
            }
            q.pop();
        }
        int sum = 0;
        for(int i = 0;i<n;++i){
            sum+=dist[i];
        }

        std::vector<int> ans(n);
        for(int i = 0;i<n;++i){
            ans[i] = -1;
        }
        ans[0] = sum;
        q.push(0);
        while(!q.empty()){
            int cur_node = q.front();
            int cur_sum = ans[cur_node];
            for(int i = 0;i<adj[cur_node].size();++i){
                if(ans[adj[cur_node][i]] == -1){
                    ans[adj[cur_node][i]] = cur_sum - 2 * num_of_nodes[adj[cur_node][i]] + n;
                    q.push(adj[cur_node][i]);
                }
            }
            q.pop();
        }

        return ans;
    }
};