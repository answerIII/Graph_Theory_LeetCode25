/*
You are given an array of strings equations that represent relationships between
variables where each string equations[i] is of length 4 and takes one of two
different forms: "xi==yi" or "xi!=yi".Here, xi and yi are lowercase letters (not
necessarily different) that represent one-letter variable names.

Return true if it is possible to assign integers to variable names so as to
satisfy all the given equations, or false otherwise.
Time complexity: O(equations.size())
Additional memory complexity: O(Alphabet)
Idea: iterate over equations, merge sets if it is '==' equation
iterate over equations, check that '!=' equations correspond to different sets
*/

#include <vector>
#include <string>

class DSU {
public:
    DSU()
        : parent_(),
          size_(26, 1) {
        parent_.reserve(26);
        for (int i = 0; i < 26; ++i) {
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
            if (size_[set1] < size_[set2]) {
                std::swap(set1, set2);
            }
            parent_[set2] = set1;
            size_[set1] += size_[set2];
        }
    }

private:
    std::vector<int> parent_;
    std::vector<int> size_;
};

class Solution {
public:
    [[nodiscard]] static bool equationsPossible(
        const std::vector<std::string>& equations) {
        DSU graph_components;
        const std::size_t equations_size = equations.size();
        for (std::size_t i = 0; i < equations_size; ++i) {
            if (equations[i][1] == '=') {
                graph_components.unionSets(equations[i][0] - 'a',
                                           equations[i][3] - 'a');
            }
        }
        for (std::size_t i = 0; i < equations_size; ++i) {
            const int var1_id = equations[i][0] - 'a';
            const int var2_id = equations[i][3] - 'a';
            if (equations[i][1] == '!' && (graph_components.findSet(var1_id) ==
                                           graph_components.findSet(var2_id))) {
                return false;
            }
        }
        return true;
    }
};
