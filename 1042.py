class Solution(object):
    def gardenNoAdj(self, n, paths):
        """
        :type n: int
        :type paths: List[List[int]]
        :rtype: List[int]
        """
        # make vertices 0-indexed (1,n -> 0,n-1)
        for i in range(len(paths)):
            paths[i][0] -= 1
            paths[i][1] -= 1

        adjacency_list = [[] for _ in range(n)]
        for u, v in paths:
            adjacency_list[u].append(v)
            adjacency_list[v].append(u)

        coloring = [0] * n # for i-th garden: flower type (1-4), 0 if not assigned

        for curr_vertex in range(n):
            forbidden_colors = set()

            # forbid colors of adjacent gardens
            for neighbor in adjacency_list[curr_vertex]:
                if coloring[neighbor] != 0:
                    forbidden_colors.add(coloring[neighbor])

            # assign the first available color
            for color in range(1, 5):
                if color not in forbidden_colors:
                    coloring[curr_vertex] = color
                    break

        return coloring
