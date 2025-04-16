class Solution(object):
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