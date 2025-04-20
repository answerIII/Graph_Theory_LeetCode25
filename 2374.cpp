class Solution {
public:
    int edgeScore(std::vector<int>& edges) {
        std::vector<int> scores(edges.size(), 0);
        int currentMaxScore = 0;
        int currentMaxScoreIndex = 0;
        for (int i = 0; i < edges.size(); ++i) {
            scores[edges[i]] += i;
            if (scores[edges[i]] > currentMaxScore) {
                currentMaxScore = scores[edges[i]];
                currentMaxScoreIndex = edges[i];
            }
        }
        return currentMaxScoreIndex;
    }
};
