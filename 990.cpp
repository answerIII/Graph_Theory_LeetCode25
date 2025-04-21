#include <vector>
#include <string>

class UnionFind {
    int* parent;
    int* rank;
public:
    UnionFind(int size) {
        parent = new int[size];
        rank = new int[size];
        for (int i = 0; i < size; ++i) {
            rank[i] = 0;
            parent[i] = i;
        }
    }
    int find(int x) {
        if (x != parent[x]) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    void uni(int x1, int x2) {
        int x1r = find(x1);
        int x2r = find(x2);
        if (x1r == x2r) {
            return;
        }
        if (rank[x1r] < rank[x2r]) {
            parent[x1r] = x2r;
        }
        else {
            parent[x2r] = x1r;
            if (rank[x1r] == rank[x2r]) {
                ++rank[x1r];
            }
        }
    }
    ~UnionFind() {
        delete[] parent;
        delete[] rank;
    }
};

class Solution {
public:
    bool equationsPossible(std::vector<std::string>& equations) {
        UnionFind uf(26);
        for (auto e : equations) {
            if (e[1] == '=') {
                int x1 = static_cast<int>(e[0] - 'a');
                int x2 = static_cast<int>(e[3] - 'a');
                uf.uni(x1, x2);
            }
        }
        for (auto e : equations) {
            if (e[1] == '!') {
                int x1 = static_cast<int>(e[0] - 'a');
                int x2 = static_cast<int>(e[3] - 'a');
                if (uf.find(x1) == uf.find(x2)) {
                    return false;
                }
            }
        }
        return true;
    }
};
