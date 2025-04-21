class Solution(object):
    def gardenNoAdj(self, n, paths):
        """
        :type n: int
        :type paths: List[List[int]]
        :rtype: List[int]
        """

        adjacency_list = [[] for _ in range(n)]
        for u, v in paths:
            adjacency_list[u].append(v)
            adjacency_list[v].append(u)