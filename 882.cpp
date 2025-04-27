// 882. Reachable Nodes In Subdivided Graph
//You are given an undirected graph (the "original graph") with n nodes labeled from 0 to n - 1. You decide to subdivide each edge in the graph into a chain of nodes, with the number of new nodes varying between each edge.

//The graph is given as a 2D array of edges where edges[i] = [ui, vi, cnti] indicates that there is an edge between nodes ui and vi in the original graph, and cnti is the total number of new nodes that you will subdivide the edge into. Note that cnti == 0 means you will not subdivide the edge.

//To subdivide the edge [ui, vi], replace it with (cnti + 1) new edges and cnti new nodes. The new nodes are x1, x2, ..., xcnti, and the new edges are [ui, x1], [x1, x2], [x2, x3], ..., [xcnti-1, xcnti], [xcnti, vi].

//In this new graph, you want to know how many nodes are reachable from the node 0, where a node is reachable if the distance is maxMoves or less.

//Given the original graph and maxMoves, return the number of nodes that are reachable from node 0 in the new graph.

class Solution
{
public:
    int reachableNodes(vector<vector<int>>& edges, int maxMoves, int n)
    {
        vector<vector<pair<int, int>>> graph(n);

        for (int i = 0; i < edges.size(); ++i)
        {
            int u = edges[i][0], v = edges[i][1], cnt = edges[i][2];
            graph[u].push_back({v, cnt});
            graph[v].push_back({u, cnt});
        }

        vector<int> dist(n, 1e9);
        priority_queue<pair<int, int>> pq;
        pq.push({0, 0});
        dist[0] = 0;

        while (!pq.empty())
        {
            auto [d, u] = pq.top();
            pq.pop();
            d = -d;

            if (d > maxMoves)
            {
                continue;
            }

            for (auto [v, cnt] : graph[u])
            {
                int nextDist = d + cnt + 1;
                if (nextDist < dist[v])
                {
                    dist[v] = nextDist;
                    pq.push({-nextDist, v});
                }
            }
        }

        int result = 0;

        for (int i = 0; i < n; ++i)
        {
            if (dist[i] <= maxMoves)
            {
                ++result;
            }
        }

        for (int i = 0; i < edges.size(); ++i)
        {
            int u = edges[i][0], v = edges[i][1], cnt = edges[i][2];
            int usedU = dist[u] > maxMoves ? 0 : maxMoves - dist[u];
            int usedV = dist[v] > maxMoves ? 0 : maxMoves - dist[v];
            result += min(cnt, usedU + usedV);
        }

        return result;
    }
};
