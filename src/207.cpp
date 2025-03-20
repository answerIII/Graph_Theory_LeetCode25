#include <vector>

class Solution {
public:

    std::vector<vector<int>> graph;
    std::vector<int> visited_arr;


    bool dfs(int course){
        if( visited_arr[course] == 1){
                return false;
            }

            if( visited_arr[course] == 2){
                return true;
            }

            visited_arr[course] = 1;

            for(int i = 0; i < graph[course].size(); ++i){
                if(!dfs(graph[course][i])){
                    return false;
                }
            }

        visited_arr[course] = 2; 
        return true;
    }

    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        graph.resize(numCourses);

        for(int i = 0; i < prerequisites.size(); ++i){
            int a = prerequisites[i][0];
            int b = prerequisites[i][1];
            graph[b].push_back(a);
        }

        visited_arr.resize(numCourses, 0);

        for(int i = 0; i < numCourses; ++i){
            if(visited_arr[i] == 0){
                if(!dfs(i)){
                    return false;
                }
            }
        }

        return true;

    }
};
