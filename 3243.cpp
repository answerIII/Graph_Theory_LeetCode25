// https://leetcode.com/problems/shortest-distance-after-road-addition-queries-i/description/

// You are given an integer n and a 2D integer array queries.
// There are n cities numbered from 0 to n - 1. Initially, there is a unidirectional road from city i to city i + 1 for all 0 <= i < n - 1.
// queries[i] = [ui, vi] represents the addition of a new unidirectional road from city ui to city vi. After each query, you need to find the length of the shortest path from city 0 to city n - 1.
// Return an array answer where for each i in the range [0, queries.length - 1], answer[i] is the length of the shortest path from city 0 to city n - 1 after processing the first i + 1 queries.

class Solution {
public:

    void initializeGraph(vector<vector<int>>& paths, vector<int>& distance, int n) {
        paths.resize(n);
        distance.resize(n);
        
        for (int i = 0; i < n - 1; i++) {
            paths[i].push_back(i + 1);
            distance[i] = i;
        }
        distance[n - 1] = n - 1;
    }

    void updateDistances(vector<vector<int>>& paths, vector<int>& distance, int start) {
        queue<int> q;
        q.push(start);
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            
            for (int next : paths[v]) {
                if (distance[v] < distance[next] - 1) {
                    distance[next] = distance[v] + 1;
                    q.push(next);
                }
            }
        }
    }
    vector<int> shortestDistanceAfterQueries(int n, vector<vector<int>>& queries) {
        vector<vector<int>> paths;
        vector<int> distance;
        vector<int> solution;
        
        initializeGraph(paths, distance, n);

        for (const auto& query : queries) 
        {
            int a = query[0];
            int b = query[1];
            
            paths[a].push_back(b);
            
            if (distance[a]< distance[b] - 1)
            {
                distance[b] = distance[a] + 1;
                updateDistances(paths, distance, b);
            }
            solution.push_back(distance.back());
        }
        return solution;
    }
};
