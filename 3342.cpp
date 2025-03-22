class Solution {
    int len = 0;
    vector<int> dist;
    vector<bool> visited;
    priority_queue<pair<int, int>, vector<pair<int, int>>, 
               greater<pair<int, int>>> pq;
    int width;
    int height;
public:
    int minTimeToReach(vector<vector<int>>& moveTime) {
        height = moveTime.size();
        width = moveTime[0].size();
        int n = width * height;
        dist.resize(n, INT_MAX);
        dist[0] = 0;
        pq.push({0, 0});
        visited.resize(n, false);
        while (pq.size() != 0) {
            int node = pq.top().second;
            int minDist = pq.top().first;
            pq.pop();
            visited[node] = true;
 
            if (node % width + 1 < width) {
                if (!visited[node + 1]) deikstr(node + 1, node, moveTime);                
            }
            if (node % width - 1 >= 0) {
                if (!visited[node - 1]) deikstr(node - 1, node, moveTime); 
            }
             if (node / width + 1 < height) {
                if (!visited[node + width]) deikstr(node + width, node, moveTime);                
            }
            if (node / width - 1 >= 0) {
                if (!visited[node - width]) deikstr(node - width, node, moveTime); 
            }            
        }  
        return dist[n - 1];      
    }

    void deikstr(int newNode, int node, vector<vector<int>>& moveTime) {
        int i = node / width;
        int j = node % width;
        int curDist = max(moveTime[newNode / width][newNode % width], dist[node]);
         curDist += (i + j) % 2 + 1;
        if (dist[newNode] == INT_MAX) {
            dist[newNode] = curDist;
            pq.push({curDist, newNode});
        } else if (dist[newNode] > curDist) {
            dist[newNode] = curDist; 
      
            vector<pair<int, int>> temp;

            while (!pq.empty()) {
                auto [value, key] = pq.top();
                pq.pop();

                if (key == newNode) {
                    value = curDist;
                }

                temp.push_back({value, key});
            }

            for (auto& p : temp) {
                pq.push(p);
            }
            pq.push({curDist, newNode});
        }
    }
};
