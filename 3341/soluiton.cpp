#include<bits/stdc++.h>
#include <queue>

using namespace std;

class Solution {
private:
    static constexpr int mx[4]{0,1,0,-1}, 
                         my[4]{1,0,-1,0};   
    const int INF = INT_MAX;


public:
    int minTimeToReach(vector<vector<int>>& moveTime) {
        int n = moveTime.size(),
            m = moveTime[0].size();
        vector<vector<int>> dist(n, vector<int>(m, INF));
        dist[0][0] = 0;

        using T = tuple<int,int,int>;
        priority_queue<T, vector<T>, greater<T>> q;
        q.push({0,0,0});

        --n;--m;
        while(!q.empty()){
            auto [d, x, y] = q.top(); q.pop();

            if(x == n && y == m){
                return d;
            }

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
