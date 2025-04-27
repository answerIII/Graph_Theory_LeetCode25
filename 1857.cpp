//1857. Largest Color Value in a Directed Graph

//There is a directed graph of n colored nodes and m edges. The nodes are numbered from 0 to n - 1.
//You are given a string colors where colors[i] is a lowercase English letter representing the 
//color of the ith node in this graph (0-indexed). You are also given a 2D array edges where edges[j] = [aj, bj] indicates that there is a directed edge from node aj to node bj.
//A valid path in the graph is a sequence of nodes x1 -> x2 -> x3 -> ... -> xk such that there is 
//a directed edge from xi to xi+1 for every 1 <= i < k. The color value of the path is the number of nodes that are colored the most frequently occurring color along that path.
//Return the largest color value of any valid path in the given graph, or -1 if the graph contains a cycle.


class Solution
{
public:
    int largestPathValue(string colors, vector<vector<int>>& edges)
    {
        int n = colors.size();
        vector<vector<int>> graph(n);
        vector<int> indegree(n, 0);

        for (int i = 0; i < edges.size(); ++i)
        {
            int u = edges[i][0], v = edges[i][1];
            graph[u].push_back(v);
            ++indegree[v];
        }

        vector<vector<int>> dp(n, vector<int>(26, 0));
        queue<int> q;

        for (int i = 0; i < n; ++i)
        {
            if (indegree[i] == 0)
            {
                q.push(i);
            }
        }

        int visited = 0;
        int result = 0;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();
            ++visited;

            ++dp[u][colors[u] - 'a'];
            result = max(result, dp[u][colors[u] - 'a']);

            for (int v : graph[u])
            {
                for (int c = 0; c < 26; ++c)
                {
                    dp[v][c] = max(dp[v][c], dp[u][c]);
                }
                --indegree[v];
                if (indegree[v] == 0)
                {
                    q.push(v);
                }
            }
        }

        return visited == n ? result : -1;
    }
};
