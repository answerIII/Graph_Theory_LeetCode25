class Solution {
public:
    vector<bool> checkIfPrerequisite(int numCourses, vector<vector<int>>& prerequisites, vector<vector<int>>& queries) {

        vector<bitset<100>> parents(numCourses, false);
        vector<vector<int>> sons(numCourses);
        vector<int> degree_in(numCourses, 0);
        bitset<100> init_parent;
        for (auto edge : prerequisites) {
            init_parent[edge[0]] = true;
            parents[edge[1]] |= init_parent;
            init_parent[edge[0]] = false;

            sons[edge[0]].push_back(edge[1]);

            ++degree_in[edge[1]];
        }

        queue<int> origin_q;
        for (int i = 0; i < numCourses; ++i) {
            if (degree_in[i] == 0) {
                origin_q.push(i);
            }
        }

        int vertex;
        while(!origin_q.empty()) {
            vertex = origin_q.front();
            origin_q.pop();
            for (int son : sons[vertex]) {
                parents[son] |= parents[vertex];
                --degree_in[son];
                if (degree_in[son] == 0) {
                    origin_q.push(son);
                }
            }
        }

        int query_count = queries.size();
        vector<bool> result = vector<bool>(query_count, false);

        for (int i = 0; i < query_count; ++i) {
            init_parent[queries[i][0]] = true;
            result[i] = (parents[queries[i][1]] & init_parent).any();
            init_parent[queries[i][0]] = false;
        }

        return result;

    }
};