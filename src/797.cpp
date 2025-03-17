class Solution {
private:
    void checkPath(vector<int>& curr_path, vector<vector<int>>& graph, int vertex) {
    
    curr_path.push_back(vertex);
    if (vertex == graph_size - 1) {
        result.push_back(curr_path);
        curr_path.pop_back();
        return;
    }

    for (int son : graph[vertex]) {
        checkPath(curr_path, graph, son);
    }
    curr_path.pop_back();
}
int graph_size;
vector<vector<int>> result;
public:
    vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
        graph_size = graph.size();
        vector<int> curr_path;
        checkPath(curr_path, graph, 0);
        return result;

    }
};