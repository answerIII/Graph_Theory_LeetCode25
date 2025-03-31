class Solution {
public:
    int minScore(int n, vector<vector<int>>& roads) {
       std::vector<std::vector<pair<int,int>>> graph(n+1);
       std::vector<int> visited(n+1, false);
       std::queue<int> q;

       for(int i=0; i<roads.size(); ++i){
        int a = roads[i][0];
        int b = roads[i][1];
        int dist = roads[i][2];
        graph[a].push_back({b, dist});//добавляем город b в список смежности а
        graph[b].push_back({a, dist});// делаем тоже самое для обеспечения bidirectional road
       } 

       q.push(1);
       visited[1] = true;
       int min_score = INT_MAX;

       while(!q.empty()){
        int city = q.front();//запоминаем вершину
        q.pop();

        for(int i=0; i<graph[city].size();++i){
            int neighbor = graph[city][i].first;
            int distance = graph[city][i].second;
            min_score = std::min(distance, min_score);

            if(!visited[neighbor]){
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
       }
       return min_score;
    }
};
