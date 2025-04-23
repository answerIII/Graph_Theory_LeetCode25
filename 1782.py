from collections import defaultdict

class Solution(object):
    def countPairs(self, n, edges, queries):
        """
        :type n: int
        :type edges: List[List[int]]
        :type queries: List[int]
        :rtype: List[int]
        """

        deg = defaultdict(lambda: 0)
        shared = defaultdict(lambda: 0)

        for u, v in edges:
            deg[u] += 1
            deg[v] += 1
            first_vertex = min(u, v)
            second_vertex = max(u, v)
            shared[(first_vertex, second_vertex)] += 1 # so (1, 2) and (2, 1) are the same
