// 3419. Minimize the Maximum Edge Weight of Graph
"""
  You are given two integers, n and threshold, as well as a directed weighted graph of n nodes numbered from 0 to n - 1. The graph is represented by a 2D integer array edges, where edges[i] = [Ai, Bi, Wi] indicates that there is an edge going from node Ai to node Bi with weight Wi.
  
  You have to remove some edges from this graph (possibly none), so that it satisfies the following conditions:
  
  Node 0 must be reachable from all other nodes.
  The maximum edge weight in the resulting graph is minimized.
  Each node has at most threshold outgoing edges.
  Return the minimum possible value of the maximum edge weight after removing the necessary edges. If it is impossible for all conditions to be satisfied, return -1.
"""
  
class Solution {
public:
    vector<vector<pair<int, int>>> buildReversedGraph(int n, const vector<vector<int>>& edges, int max_weight) 
    {
        vector<vector<pair<int, int>>> reversed_graph(n);
        for (const auto& edge : edges) 
        {
            int source = edge[0];
            int distanation = edge[1];
            int weight = edge[2];
            if (weight <= max_weight) 
            {
                reversed_graph[distanation].emplace_back(source, weight); 
            }
        }
        return reversed_graph;
    }
    
    bool bfs(const vector<vector<pair<int, int>>>& reversed_graph, int threshold, int n) 
    {
        vector<int> out_degree(n, 0);
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(0);
        visited[0] = true;
        int cnt = 1;
        
        while (!q.empty()) 
        {
            int u = q.front();
            q.pop();
            
            for (const auto& neighbor_pair : reversed_graph[u]) 
            {
                int v = neighbor_pair.first;
                if (out_degree[v] < threshold)
                {
                    if (!visited[v]) 
                    {
                        visited[v] = true;
                        ++cnt;
                        q.push(v);
                    }
                    ++out_degree[v];
                }
            }
        }
        
        return cnt == n;
    }
    
    bool canReachAllNodes(int n, const vector<vector<int>>& edges, int threshold, int max_weight) 
    {
        vector<vector<pair<int, int>>> reversed_graph = buildReversedGraph(n, edges, max_weight);
        return bfs(reversed_graph, threshold, n);
    }
    
    int minMaxWeight(int n, vector<vector<int>>& edges, int threshold) {
        int left = 1;
        int right = 10000000;
        int answer = -1;
        
        while (left <= right) 
        {
            int mid = left + (right - left) / 2;
            if (canReachAllNodes(n, edges, threshold, mid)) {
                answer = mid;
                right = mid - 1;
            } else 
            {
                left = mid + 1;
            }
        }
        
        return answer;
    }
};
