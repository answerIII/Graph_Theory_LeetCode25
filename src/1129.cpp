#include <vector> 
#include <queue>

class Solution {
public:
    std::vector<int> shortestAlternatingPaths(int n, std::vector<std::vector<int>>& redEdges, std::vector<std::vector<int>>& blueEdges) {
        std::vector<std::vector<std::pair<int, int>>> graph(n);

        for(int i = 0; i < redEdges.size(); ++i){
            graph[redEdges[i][0]].emplace_back(redEdges[i][1], 0); 
        }

        for(int i = 0; i < blueEdges.size(); ++i){
            graph[blueEdges[i][0]].emplace_back(blueEdges[i][1], 1);  
        }

        std::queue<std::pair<int, int>> q;
        q.push(std::make_pair(0, 0));  
        q.push(std::make_pair(0, 1)); 

        std::vector<std::vector<int>> dist(n, std::vector<int>(2, -1));
        dist[0][0] = 0;
        dist[0][1] = 0;

        while(!q.empty()){
            int node = q.front().first;
            int color = q.front().second;
            q.pop();

            int next_color = 1 - color;  

            for(int i = 0; i < graph[node].size(); ++i){
                int neighbor = graph[node][i].first;
                int edgeColor = graph[node][i].second;

                if(edgeColor == next_color && dist[neighbor][edgeColor] == -1){
                    dist[neighbor][edgeColor] = 1 + dist[node][color];
                    q.push(std::make_pair(neighbor, edgeColor));
                }
            }
        }

        std::vector<int> result(n, -1);

        for(int i = 0; i < n; ++i){
            if(dist[i][0] != -1 && dist[i][1] != -1){
                result[i] = std::min(dist[i][0], dist[i][1]);
            } else if(dist[i][0] != -1){
                result[i] = dist[i][0];
            } else if(dist[i][1] != -1){
                result[i] = dist[i][1];
            }
        }

        return result;
    }
};

