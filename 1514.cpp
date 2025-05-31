#include <limits.h>
#include <stdio.h>

int maxProb(int n, double Prob[], bool sptSet[])
{
    double max = 0;
    int max_index;
    for (int v = 0; v < n; v++){
        if (sptSet[v] == false && Prob[v] >= max){
            max = Prob[v];
            max_index = v;
        }
    }
    return max_index;
}


class Solution {
public:
    double maxProbability(int n, vector<vector<int>>& edges, vector<double>& succProb, int start_node, int end_node) {
        
        vector<vector<pair<double, int>>> adj(n);
        for(int i = 0;i<edges.size();++i){
            adj[edges[i][0]].push_back({succProb[i], edges[i][1]});
            adj[edges[i][1]].push_back({succProb[i], edges[i][0]});
        }   
        priority_queue<pair<double, int>> pq;
        vector<double> Prob(n, 0);
    
        Prob[start_node] = 1;
        pq.push({1, start_node});

        while (!pq.empty())
        {
            int node = pq.top().second;
            double dis = pq.top().first;
            pq.pop();

            for (auto it : adj[node])
            {
                int v = it.second;
                double w = it.first;
                if (dis * w > Prob[v])
                {
                    Prob[v] = dis * w;
    
                    pq.push({dis * w, v});
                }
            }
        }
        double ans = Prob[end_node];
        return ans;
    }
};