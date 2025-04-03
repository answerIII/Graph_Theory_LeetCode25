// https://leetcode.com/problems/get-watched-videos-by-your-friends/description/
// There are n people, each person has a unique id between 0 and n-1. Given the arrays watchedVideos and friends, where watchedVideos[i] and friends[i] 
// contain the list of watched videos and the list of friends respectively for the person with id = i.
// Level 1 of videos are all watched videos by your friends, level 2 of videos are all watched videos by the friends of your friends and so on. 
// In general, the level k of videos are all watched videos by people with the shortest path exactly equal to k with you. 
// Given your id and the level of videos, return the list of videos ordered by their frequencies (increasing). For videos with the same frequency order them alphabetically from least to greatest. 

class Solution {
public:
     void bfsLevelFriends(vector<vector<int>>& friends, int id, int level, vector<int>& levelFriends) 
     {
        int n = friends.size();
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(id);
        visited[id] = true;
        
        while (level-- > 0) 
        {
            int size = q.size();
            for (int i = 0; i < size; ++i)
            {
                int person = q.front();
                q.pop();
                for (int friendId : friends[person]) 
                {
                    if (!visited[friendId]) 
                    {
                        visited[friendId] = true;
                        q.push(friendId);
                    }
                }
            }
        }
        
        while (!q.empty()) 
        {
            levelFriends.push_back(q.front());
            q.pop();
        }
    }
    
    unordered_map<string, int> countVideoFrequency(vector<vector<string>>& watchedVideos, vector<int>& levelFriends) 
    {
        unordered_map<string, int> videoFrequency;
        for (int person : levelFriends) 
        {
            for (const string& video : watchedVideos[person]) 
            {
                ++videoFrequency[video];
            }
        }
        return videoFrequency;
    }
    
    vector<string> sortVideosByFrequency(unordered_map<string, int>& videoFrequency) 
    {
        vector<pair<int, string>> sortedVideos;
        for (const auto& entry : videoFrequency) 
        {
            sortedVideos.emplace_back(entry.second, entry.first);
        }
        
        sort(sortedVideos.begin(), sortedVideos.end(), [](const pair<int, string>& a, const pair<int, string>& b) 
        {
            return a.first == b.first ? a.second < b.second : a.first < b.first;
        });
        
        vector<string> result;
        for (const auto& entry : sortedVideos) 
        {
            result.push_back(entry.second);
        }
        return result;
    }
    vector<string> watchedVideosByFriends(vector<vector<string>>& watchedVideos, vector<vector<int>>& friends, int id, int level) {
        vector<int> levelFriends;
        bfsLevelFriends(friends, id, level, levelFriends);
        unordered_map<string, int> videoFrequency = countVideoFrequency(watchedVideos, levelFriends);
        return sortVideosByFrequency(videoFrequency);
    }
};
