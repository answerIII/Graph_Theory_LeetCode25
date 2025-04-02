// https://leetcode.com/problems/maximum-total-importance-of-roads/description/

// You are given an integer n denoting the number of cities in a country. The cities are numbered from 0 to n - 1.
// You are also given a 2D integer array roads where roads[i] = [ai, bi] denotes that there exists a bidirectional road connecting cities ai and bi.
// You need to assign each city with an integer value from 1 to n, where each value can only be used once. The importance of a road is then defined as the sum of the values of the two cities it connects.
// Return the maximum total importance of all roads possible after assigning the values optimally.

class Solution {
public:
    void heapify(vector<pair<long, long>>& arr, int n, int i)
    {
        long long smallest = i;
        long long left = 2 * i + 1;
        long long right = 2 * i + 2;

        if (left < n && arr[left].first < arr[smallest].first)
        {
            smallest = left;
        }
        if (right < n && arr[right].first < arr[smallest].first)
        {
            smallest = right;
        }
        if (smallest != i) 
        {
            swap(arr[i], arr[smallest]);
            heapify(arr, n, smallest);
        }
    }

    void heapSort(vector<pair<long, long>>& arr)
    {
        int n = arr.size();
        for (long long i = n / 2 - 1; i >= 0; --i)
        {
            heapify(arr, n, i);
        }
        for (long long i = n - 1; i >=0; --i) 
        {
            swap(arr[0], arr[i]);
            heapify(arr, i, 0);
        }
    }

    long long maximumImportance(int n, vector<vector<int>>& roads) {

        vector<long long> degree(n, 0);
        for (const auto& road : roads) {
            ++degree[road[0]];
            ++degree[road[1]];
        }
    
        vector<pair<long, long>> cityDegree;
        for (long long i = 0; i < n;++i) {
            cityDegree.push_back({degree[i], i});
        }
    
        heapSort(cityDegree);
    
        vector<long long> cityValue(n, 0);
        long long value = n;
        for (const auto& [deg,city] : cityDegree) {
            cityValue[city] =value--;
        }
    
        long long total = 0;
        for (const auto& road : roads) {
            total += cityValue[road[0]] + cityValue[road[1]];
        }
    
        return total;
    }
};
