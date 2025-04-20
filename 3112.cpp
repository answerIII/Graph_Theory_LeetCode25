// 3112. Minimum Time to Visit Disappearing Nodes
// There is an undirected graph of n nodes. You are given a 2D array edges, where edges[i] = [ui, vi, lengthi] describes an edge between node ui and node vi with a traversal time of lengthi units.
// Additionally, you are given an array disappear, where disappear[i] denotes the time when the node i disappears from the graph and you won't be able to visit it.
// Note that the graph might be disconnected and might contain multiple edges.
// Return the array answer, with answer[i] denoting the minimum units of time required to reach node i from node 0. If node i is unreachable from node 0 then answer[i] is -1.

class Solution {
public:
    struct Edge{

        int u;
        int v;
        int l;

        Edge(int u, int v, int l): u(u), v(v), l(l){}
    };

    vector<int> minimumTime(int n, vector<vector<int>>& edges, vector<int>& disappear) {
        vector<int> ans(n, -1);
        vector<int> dist(n, INT_MAX);
        dist[0] = 0;

        vector<vector<Edge>> adj(n);
        for(const auto& edge: edges)
        {
            int u = edge[0];
            int v = edge[1];
            int l = edge[2];
            adj[u].emplace_back(u, v, l);
            adj[v].emplace_back(v, u, l);
        }

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.emplace(0, 0);

        while(!pq.empty())
        {
            auto [cur_dist, u] = pq.top();
            pq.pop();
            if((cur_dist > dist[u] || cur_dist >= disappear[u]))
            {
                continue;
            }
                for(const Edge& edge : adj[u])
                {
                    int v = edge.v;
                    int d = cur_dist + edge.l;
                    if(d < dist[v] && d < disappear[v])
                    {
                        dist[v] = d;
                        pq.emplace(d, v);
                    }
                }
            
        }

         for (int i = 0; i < n; ++i) 
        {
            if (dist[i] < disappear[i]) 
            {
            ans[i] = dist[i];
            }
        }
        ans[0] = 0; 
        return ans;

    }
};
