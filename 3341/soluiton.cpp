#include<bits/stdc++.h>
#include <queue>

using namespace std;

class Solution {
public:
    int minTimeToReach(vector<vector<int>>& moveTime) {
        const int mx[4]{0,1,0,-1}, 
                  my[4]{1,0,-1,1};   

        vector<vector<int>> dist(moveTime.size(), vector<int>(moveTime[0].size(), INT_MAX));
        dist[0][0] = 0;

        using T = tuple<int,int,int>;
        priority_queue<T, vector<T>, greater<T>> q;
        q.push({0,0,0});

        while(!q.empty()){
            auto [d, x, y] = q.top(); q.pop();

            for(int i = 0; i < 4; ++i){
                int nx = x + mx[i], ny = y + my[i];

                if(nx < 0 || ny < 0 || nx >= dist.size() || ny >= dist[0].size()){
                    continue;
                }

                int next_move = max(dist[x][y], moveTime[nx][ny]) + 1;
                if(dist[nx][ny] > next_move){
                    dist[nx][ny] = next_move;
                    q.push({next_move, nx, ny});
                }
            }
        }

        return dist.back().back();
    }
};

int main(){
    Solution sol;
    int n, m; cin >> n >> m;

    vector<vector<int>> v(n, vector<int>(m));
    for(auto &a : v){
        for(auto &el : a){
            cin >> el;
        }
    }

    cout << sol.minTimeToReach(v);
    return 0;
}
