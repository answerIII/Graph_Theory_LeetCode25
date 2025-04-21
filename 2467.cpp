class Solution {
public:

    //bob's path
    void DFSBob(int u, int prev, int depth, bool& end, std::vector<std::vector<int>>& adjacency, std::unordered_map<int, int>& bobs_path) {
        bobs_path.emplace(u, depth);
        if (u == 0) {
            end = true;
            return;
        }
        for (auto v : adjacency[u]) {
            if (v != prev) {
                DFSBob(v, u, depth + 1, end, adjacency, bobs_path);
            }
            if (end) {
                break;
            }
        }
        if (!end) {
            bobs_path.erase(u);
        }
    }
    // if alice is at a vertex u of depth d, and it is in bob's route, 
    // then we need to see if bob can be at this vertex by this time
    void DFSAlice(int u, int prev, int depth, std::vector<std::vector<int>>& adjacency, 
        std::unordered_map<int, int>& bobs_path, std::vector<int>& amount, int score, int& maxScore) {
        
        auto it = bobs_path.find(u);
        if (it != bobs_path.end()) { // starting with zero for Alice - works fine
            if (depth < it->second) {
                score += amount[u];
            }
            else if (depth == it->second) {
                score += amount[u] / 2;
            }
        }
        else {
            score += amount[u];
        }
        for (auto v : adjacency[u]) {
            if (v != prev) {
                DFSAlice(v, u, depth + 1, adjacency, bobs_path, amount, score, maxScore);
            }
        }
        if (adjacency[u].empty()) {
            maxScore = std::max(score, maxScore);
        }
    }

    int mostProfitablePath(std::vector<std::vector<int>>& edges, int bob, std::vector<int>& amount) {
        std::vector<std::vector<int>> adjacency(amount.size());
        for (int i = 0; i < edges.size(); ++i) {
            adjacency[edges[i][0]].push_back(edges[i][1]);
            adjacency[edges[i][1]].push_back(edges[i][0]);
        }
        std::unordered_map<int, int> bobs_path;
        bool end = false;
        int maxScore = 0;
        DFSBob(bob, -1, 0, end, adjacency, bobs_path);
        DFSAlice(0, -1, 0, adjacency, bobs_path, amount, 0, maxScore);
        return maxScore;
    }
};
