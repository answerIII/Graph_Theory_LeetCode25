class Solution {
public:
    bool check_node(vector<vector<int>>& adj, int n, int node, vector<int>& colours){
        queue<int> q;
        q.push(node);
        colours[node] = 1;
        while(!q.empty()){
            int tmp_node = q.front();
            for(int i = 0;i<adj[tmp_node].size();++i){
                if(colours[adj[tmp_node][i]] == colours[tmp_node]){
                    return false;
                }
                if(colours[adj[tmp_node][i]] == 0){
                    if(colours[tmp_node] == 1){
                        colours[adj[tmp_node][i]] = 2;
                    }
                    else{
                        colours[adj[tmp_node][i]] = 1;
                    }
                    q.push(adj[tmp_node][i]);
                }
            }

            q.pop();
        }
        return true;
    }
    bool possibleBipartition(int n, vector<vector<int>>& dislikes) {
        vector<vector<int>> adj(n+1);
        for(int i = 0;i<dislikes.size();++i){
            adj[dislikes[i][0]].push_back(dislikes[i][1]);
            adj[dislikes[i][1]].push_back(dislikes[i][0]);
        }      
    vector<int> colours (n+1);
    for (int i = 0; i < n+1; ++i) {
        colours[i] = 0;
    }
    for(int i = 0;i<n+1;++i){
        if(colours[i]==0){
            if(!check_node(adj,n,i,colours)){
                return false;
            }
        }
    }
    return true;
    }
};