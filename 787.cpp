class Solution {
public:
    int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
        unordered_map<int, vector<pair<int, int>>> flight_map;

        for (const auto& flight : flights) {
            int from = flight[0];  
            int to = flight[1];    
            int price = flight[2]; 

            flight_map[from].emplace_back(to, price);
        }

        queue<pair<int, int>> que;
        que.push({src, 0});
        vector<int> min_value(n, INT_MAX);
        min_value[src] = 0;

        for (int i = 0; i < k + 1; ++i) {
            int size = que.size();
            while (size-- != 0) {
                pair<int, int> root = que.front();
                que.pop();
                for (auto pair : flight_map[root.first]) {
                    int min_val = min_value[pair.first];
                    min_value[pair.first] = min(min_value[pair.first], root.second + pair.second);
                    if (min_val != min_value[pair.first]) {
                        que.push({pair.first, min_value[pair.first]});
                    }    
                }
            }
        }
        if (min_value[dst] == INT_MAX) {
            return -1;
        }
        return min_value[dst];
    }
};
