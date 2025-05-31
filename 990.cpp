class Solution {
public:
    int find_member(int mychar, vector<vector<char>> sets){
        for(int i = 0;i<sets.size();++i){
            for(int j = 0;j<sets[i].size();++j){
                if(sets[i][j] == mychar){
                    return i;
                }
            }
        }
        return -1;
    }
    void create_set(vector<vector<char>>& sets, char A, char B){
        sets.push_back({A,B});
    }
    void add_member(vector<vector<char>>& sets, char A, int idx){
        sets[idx].push_back(A);
    }
    void merge_sets(vector<vector<char>>& sets, int idx_1, int idx_2){
        if(idx_1 > idx_2){
            int tmp = idx_1;
            idx_1 = idx_2;
            idx_2 = tmp;
        }
        for(int i = 0;i<sets[idx_2].size();++i){
            sets[idx_1].push_back(sets[idx_2][i]);
        }
        sets[idx_2] = sets[sets.size()-1];
        sets.pop_back();
    }
    bool equationsPossible(vector<string>& equations) {
        vector<vector<char>> sets;
        for(int i = 0;i<equations.size();++i){
            if(equations[i][1] == '='){
                char A = equations[i][0];
                char B = equations[i][3];
                int idx_1 = find_member(A, sets);
                int idx_2 = find_member(B, sets);
                if(A!=B){
                    if(idx_1 == -1 && idx_2 == -1){
                        create_set(sets, A, B);
                    }
                    if(idx_1 != -1 && idx_2 == -1){
                        add_member(sets, B, idx_1);
                    }
                    if(idx_1 == -1 && idx_2 != -1){
                        add_member(sets, A, idx_2);
                    }
                    if(idx_1 != -1 && idx_2 != -1 && idx_1 != idx_2){
                        merge_sets(sets, idx_1, idx_2);
                    }
                }
            }
        }
        for(int i = 0;i<equations.size();++i){
            if(equations[i][1] == '!'){
                char A = equations[i][0];
                char B = equations[i][3];
                int idx_1 = find_member(A, sets);
                int idx_2 = find_member(B, sets);
                std::cout<<idx_1<<" "<<idx_2<<"\n";
                if(A==B){
                    return false;
                }
                if(idx_1 == idx_2 && idx_1 != -1 && idx_2 != -1){
                    return false;
                }
            }
        }
        return true;
    }
};