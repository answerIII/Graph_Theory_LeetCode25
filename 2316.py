class Solution(object):
    @staticmethod
    def dfs(curr_vert, adjacency_list, is_visited, curr_conn_comp):
        is_visited[curr_vert] = True

        curr_conn_comp.append(curr_vert)

        for adj_vertex in adjacency_list[curr_vert]:
            if not is_visited[adj_vertex]:
                Solution.dfs(adj_vertex, adjacency_list, is_visited, curr_conn_comp)

    def countPairs(self, n, edges):
        """
        :type n: int
        :type edges: List[List[int]]
        :rtype: int
        """

        adjacency_list = [[] for i in range(n)] # for i-th vertex: [v0, v4, ..., v.] <- all vertices adjacent to vi
        for u, v in edges:
            adjacency_list[u].append(v)
            adjacency_list[v].append(u)

        is_visited = [False] * n # for each vertex: True/False
        all_conn_comps = []

        # find all connected components
        for curr_vertex in range(n):
            if not is_visited[curr_vertex]:
                curr_conn_comp = []
                Solution.dfs(curr_vertex, adjacency_list, is_visited, curr_conn_comp)
                all_conn_comps.append(curr_conn_comp)

        # as we don't need vertices in components (only their sizes to say [3] -- [5] <- 3*5 pairs), we'll store just sizes
        all_conn_comps_sizes = [len(curr_conn_comp) for curr_conn_comp in all_conn_comps]

        total_unreachable_pairs = 0
        for i in range(len(all_conn_comps_sizes)):
            curr_conn_comp_size = all_conn_comps_sizes[i]
            other_part_of_graph_size = n - curr_conn_comp_size
            total_unreachable_pairs += curr_conn_comp_size * other_part_of_graph_size
        
        total_unreachable_pairs //= 2 # each pair is counted twice ([3] -- [5] and [5] -- [3])

        return total_unreachable_pairs