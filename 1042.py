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