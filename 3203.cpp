class Solution {
public:

    //выбирая какое-то ребро (a, b), наидлиннейший путь (диаметр) в полученном графе будет равен max(r[a]+r[b]+1, d(t1), d(t2)), 
    // где r[u] - максимальная длина пути, который начинается в r[u], остальные слагаемые - диаметры деревьев t1 и t2)
    // т.е d(t) = max(r[a] + r[b] + 1, d(t1), d(t2))
    // чтобы найти min[d(t)], нужно найти вершины a и b, для которых 
    // r[a] и r[b] минимально. Чтобы это сделать, нужно для каждой вершины u найти r[u], попутно посчитать диаметры, далее очевидно.

    void DFS(int u, int prev, int depth, const std::vector<std::vector<int>>& adjacency, int& maxDepth, int& maxDepthV) {
        if (depth > maxDepth) {
            maxDepth = depth;
            maxDepthV = u;
        }
        for (auto v : adjacency[u]) {
            if (v != prev) {
                DFS(v, u, depth + 1, adjacency, maxDepth, maxDepthV);
            }
        }
    }
    void fillAdj(const std::vector<std::vector<int>>& edges, std::vector<std::vector<int>>& adjacency) {
        for (int i = 0; i < edges.size(); ++i) {
            adjacency[edges[i][0]].push_back(edges[i][1]);
            adjacency[edges[i][1]].push_back(edges[i][0]);
        }
    }
    int diameter(const std::vector<std::vector<int>>& adjacency) {
        int maxDepth = 0;
        int diamBeginV = 0;
        int diamEndV = 0;
        DFS(0, -1, 0, adjacency, maxDepth, diamBeginV);
        maxDepth = 0;
        DFS(diamBeginV, -1, 0, adjacency, maxDepth, diamEndV);
        return maxDepth;

    }

    //TODO: посчитать диаметры деревьев, в каждом дереве найти диаметр (выписать в порядке обхода вершин)
    int minimumDiameterAfterMerge(std::vector<std::vector<int>>& edges1, std::vector<std::vector<int>>& edges2) {
        std::vector<std::vector<int>> adjacency1(edges1.size() + 1);
        std::vector<std::vector<int>> adjacency2(edges2.size() + 1);

        fillAdj(edges1, adjacency1);
        fillAdj(edges2, adjacency2);
        int d1 = diameter(adjacency1) + 1; // +1, потому что d1 - количество вершин в диаметре, а не ребер
        int d2 = diameter(adjacency2) + 1;
        return std::max(d1 / 2 + d2 / 2 + 1, std::max(d1, d2));
    }
};
