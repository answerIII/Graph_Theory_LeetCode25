class Solution(object):
    @staticmethod
    def mark_visited_using_iterative_dfs(curr_vert, adjacency_list, n):
        is_visited = [False] * n
        stack = [curr_vert]
        while len(stack) > 0:
            curr_vert = stack.pop()

            if not is_visited[curr_vert]:
                is_visited[curr_vert] = True
                for adj_vertex in adjacency_list[curr_vert]:
                    if not is_visited[adj_vertex]:
                        stack.append(adj_vertex)
        return is_visited

    def remainingMethods(self, n, k, invocations):
        """
        :type n: int
        :type k: int
        :type invocations: List[List[int]]
        :rtype: List[int]
        """

        adjacency_list = [[] for i in range(n)] # for i-th vertex: [v0, v4, ..., v.] <- all vertices adjacent to vi
        for u, v in invocations:
            adjacency_list[u].append(v)

        # find all 'bad' vertices
        is_visited = Solution.mark_visited_using_iterative_dfs(k, adjacency_list, n)

        any_external_vertex_calls_into_bad = False
        for i in range(n):
            if not is_visited[i]:               # external vertex
                for child in adjacency_list[i]: # look at all the chlrdren
                    if is_visited[child]:       # our child is in bad set
                        any_external_vertex_calls_into_bad = True
                        break
        no_removal = any_external_vertex_calls_into_bad

        vertices_remaining_after_removal = []
        if no_removal:
            vertices_remaining_after_removal = list(range(n))
        else:
            vertices_remaining_after_removal = [i for i in range(n) if not is_visited[i]]

        return vertices_remaining_after_removal