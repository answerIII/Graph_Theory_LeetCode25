from typing import List
from collections import deque, defaultdict


class Solution:
    def longestCycle(self, edges: List[int]) -> int:
        n = len(edges)
        connections = []
        for i in range(n):
            if edges[i] != -1:
                connections.append([i, edges[i]])

        graph = defaultdict(list)
        for u, v in connections:
            graph[u].append(v)

        
        return graph


sol = Solution()
edges = [3, 3, 4, 2, 3]
print(sol.longestCycle(edges))

sol = Solution()
edges = [2, -1, 3, 1]
print(sol.longestCycle(edges))
