/*
You are given an integer n. There is an undirected graph with n vertices,
numbered from 0 to n - 1. You are given a 2D integer array edges where edges[i]
= [ai, bi] denotes that there exists an undirected edge connecting vertices ai
and bi.

Return the number of complete connected components of the graph.

A connected component is a subgraph of a graph in which there exists a path
between any two vertices, and no vertex of the subgraph shares an edge with a
vertex outside of the subgraph.

A connected component is said to be complete if there exists an edge between
every pair of its vertices.
Time complexity: O(E + V)
Additional memroy complexity: O(V)
Idea: iterate through edges merge sets if vertices are in different sets
*/

#include <vector>

class DSU {
public:
    explicit DSU(int n)
        : parent_(),
          vertices_(n, 1),
          edges_(n, 0) {
        parent_.reserve(n);
        for (int i = 0; i < n; ++i) {
            parent_.push_back(i);
        }
    }

    [[nodiscard]] int findSet(int vertex) noexcept {
        if (parent_[vertex] == vertex) {
            return vertex;
        }
        return parent_[vertex] = findSet(parent_[vertex]);
    }

    void unionSets(int vertex1, int vertex2) noexcept {
        int set1 = findSet(vertex1);
        int set2 = findSet(vertex2);
        if (set1 != set2) {
            if (vertices_[set1] < vertices_[set2]) {
                std::swap(set1, set2);
            }
            parent_[set2] = set1;
            vertices_[set1] += vertices_[set2];
            edges_[set1] += edges_[set2] + 1;
        } else {
            ++edges_[set1];
        }
    }

    [[nodiscard]] int completeComponentsAmount() const noexcept {
        int complete_components = 0;
        for (int i = 0, total_vertices = static_cast<int>(parent_.size());
             i < total_vertices; ++i) {
            if (parent_[i] == i &&
                (vertices_[i] * (vertices_[i] - 1) / 2 == edges_[i])) {
                ++complete_components;
            }
        }
        return complete_components;
    }

private:
    std::vector<int> parent_;
    std::vector<int> vertices_;
    std::vector<int> edges_;
};

class Solution {
public:
    [[nodiscard]] static int countCompleteComponents(
        int n, const std::vector<std::vector<int>>& edges) {
        DSU graph_components(n);
        for (const auto& edge : edges) {
            graph_components.unionSets(edge[0], edge[1]);
        }
        return graph_components.completeComponentsAmount();
    }
};