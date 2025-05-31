class Solution {
public:
    int closestMeetingNode(vector<int>& edges, int node1, int node2) {
        int sz = edges.size();
        vector<int> dist1(sz);
        vector<int> dist2(sz);

        vector<bool> visited(sz);

        for(int i = 0;i<sz;++i){
            dist1[i] = -1;
        }
        dist1[node1] = 0;
        visited[node1] = true;
        int cur_node = node1;

        while(edges[cur_node] != -1 && !visited[edges[cur_node]]){
            int cur_distance = dist1[cur_node];
            cur_node = edges[cur_node];
            visited[cur_node] = true;
            if(dist1[cur_node] == -1){
                dist1[cur_node] = cur_distance + 1;
            }
        }

        for(int i = 0;i<sz;++i){
            dist2[i] = -1;
            visited[i] = false;
        }
        dist2[node2] = 0;
        visited[node2] = true;
        cur_node = node2;
        while(edges[cur_node] != -1 && !visited[edges[cur_node]]){
            int cur_distance = dist2[cur_node];
            cur_node = edges[cur_node];
            visited[cur_node] = true;
            if(dist2[cur_node] == -1){
                dist2[cur_node] = cur_distance + 1;
            }
        }

        int mindist = 2*sz;
        int ans = -1;
        for(int i = 0;i<sz;++i){
            if(mindist>max(dist1[i], dist2[i]) && dist1[i]!=-1 && dist2[i]!=-1){
                ans = i;
                mindist = max(dist1[i], dist2[i]);
            }
        }
        return ans;
    }
};