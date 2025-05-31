class Solution {
public:
    int distance_update(int n, int start, int end, vector<int>& dist, vector<vector<int>>& adj){
        adj[start].push_back(end);
        priority_queue<int> pq;
        if(dist[start]+1 < dist[end]){
            dist[end] = dist[start]+1;
            pq.push(end);
        }
        while(!pq.empty()){
            int tmp_node = pq.top();
            pq.pop();
            for(int i = 0;i<adj[tmp_node].size();++i){
                if(dist[tmp_node]+1 < dist[adj[tmp_node][i]]){
                    dist[adj[tmp_node][i]] = dist[tmp_node]+1;
                    pq.push(adj[tmp_node][i]);
                }
            }
        }

        return dist[n-1];
    }
    vector<int> shortestDistanceAfterQueries(int n, vector<vector<int>>& queries) {
        vector<int> ans(queries.size());
        vector<int> dist(n);
        for(int i = 0;i<n;++i){
            dist[i] = i;
        }
        vector<vector<int>> adj(n);
        for(int i = 0;i<n-1;++i){
            adj[i].push_back(i+1);
        }

        for(int i = 0;i<queries.size();++i){
            ans[i] = distance_update(n, queries[i][0], queries[i][1], dist, adj);
        }
        return ans;
    }
};