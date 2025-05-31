class Solution {
public:
    bool canVisitAllRooms(vector<vector<int>>& rooms) {
        vector<bool> reachable (rooms.size());
        queue<int> q;
        reachable[0] = true;
        q.push(0);

        int current_room;
        while(!q.empty()){
            current_room = q.front();
            for(int i = 0;i<rooms[current_room].size();++i){
                if(reachable[rooms[current_room][i]] == false){
                    reachable[rooms[current_room][i]] = true;
                    q.push(rooms[current_room][i]);
                }
            }
            q.pop();
        }
        std::cout<<reachable.size();
        for(int i = 0;i<rooms.size();++i){
            if(reachable[i] == false){
                return false;
            }
        }
        return true;
    }
};
