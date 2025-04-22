#include <vector>
#include <queue>

class Solution {
public:
    long long countPairs(int n, std::vector<std::vector<int>>& edges) {
        std::vector<int>* adj = new std::vector<int>[n];
        for (auto& e : edges) {
            adj[e[0]].push_back(e[1]);
            adj[e[1]].push_back(e[0]);
        }
        bool* visited = new bool[n];
        for (int i = 0; i < n; ++i) {
            visited[i] = false;
        }
        std::vector<int> subgraphSizes;
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                std::queue<int> q;
                q.push(i);
                visited[i] = true;
                int size(1);
                while (!q.empty()) {
                    int node = q.front();
                    q.pop();
                    for (auto next : adj[node]) {
                        if (!visited[next]) {
                            visited[next] = true;
                            q.push(next);
                            ++size;
                        }
                    }
                }
                subgraphSizes.push_back(size);
            }
        }
        long long total(0);
        long long sqrs(0);
        for (auto i : subgraphSizes) {
            total += static_cast<long long>(i);
            sqrs += static_cast<long long>(i) * static_cast<long long>(i);
        }
        delete[] adj;
        delete[] visited;
        return (total * total - sqrs) / 2;
    }
};
