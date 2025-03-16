class Solution {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        vector<int> inDegree(numCourses, 0);

        for (const auto& prer : prerequisites) {
            adj[prer[1]].push_back(prer[0]);
            inDegree[prer[0]]++; 
        }

        queue<int> q;
        vector<int> topoOrder;
        topoOrder.reserve(numCourses); 

           for (int i = 0; i < numCourses; ++i) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }

        while (!q.empty()) {
            int node = q.front();
            q.pop();
            topoOrder.push_back(node);

            for (int neighbor : adj[node]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) { 
                    q.push(neighbor);
                }
            }
        }

        return (topoOrder.size() == numCourses) ? topoOrder : vector<int>{};
    }    
};

