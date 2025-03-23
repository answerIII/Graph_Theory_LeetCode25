class Solution {
public:
    int edgeScore(vector<int>& edges) {
        int n = edges.size();
        std::vector score(n, 0);

        for(int i=0; i<n; ++i){
            score[edges[i]] += i;
        }

        long long max_score = -1;
        int highest_score = -1;

        for(int i=0; i<n; ++i){
            if(max_score < score[i]){
                max_score = score[i];
                highest_score = i;
            }
        }
    return highest_score;
    }
};
