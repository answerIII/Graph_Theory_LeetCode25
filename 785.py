class Solution(object):
    @staticmethod
    def run_dfs_and_color_vertices(curr_vert, adjacency_list, coloring):
        coloring[curr_vert] = 1

        stack = [curr_vert]

        while len(stack) > 0:
            curr_vert = stack.pop()

            for adj_vertex in adjacency_list[curr_vert]:
                if coloring[adj_vertex] == 0: # not visited
                    coloring[adj_vertex] = -coloring[curr_vert]
                    stack.append(adj_vertex)
                elif coloring[adj_vertex] == coloring[curr_vert]:
                    return False
        return True


    def isBipartite(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: bool
        """
        
        adjacency_list = graph
        n = len(graph)
        coloring = [0] * n # 0 for not visited, (-1, 1) for colors

        for curr_vertex in range(n):
            if coloring[curr_vertex] == 0: # not visited
                if not Solution.run_dfs_and_color_vertices(curr_vertex, adjacency_list, coloring):
                    return False
        return True