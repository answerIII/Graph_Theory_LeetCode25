class Solution {
    vector<int> size;
    vector<int> parent;
public:
    int minSwapsCouples(vector<int>& row) {
        int n = row.size();
        parent.resize(n);
        size.resize(n, 1);

        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
        for (int i = 0; i < n; i += 2) {
            unite(row[i], row[i + 1]);
            unite(i, i + 1);
        }

        int count = 0;
        for (int i = 0; i < n; ++i) {
            if (parent[i] == i) { 
                if (size[i] != 2) {
                    count += size[i] / 2 - 1;
                }
            }
        }
        return count;  
    }

    int find(int x){
        if (parent[x] == x) { return x; }
        return parent[x] = find(parent[x]);
    }

    void unite(int x, int y){
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            parent[rootX] = rootY;
            size[rootY] += size[rootX];
        }
    }   
};
