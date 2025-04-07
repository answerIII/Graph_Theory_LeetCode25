from typing import List
from collections import deque, defaultdict


class Solution:
    def makeConnected(self, n: int, connections: List[List[int]]) -> int:
        if len(connections) < n - 1:
            return -1

        graph = defaultdict(list)
        for u, v in connections:
            graph[u].append(v)
            graph[v].append(u)

        visited = [False] * n

        def bfs(start):
            queue = deque([start])
            visited[start] = True
            while queue:
                node = queue.popleft()
                for neighbor in graph[node]:
                    if not visited[neighbor]:
                        visited[neighbor] = True
                        queue.append(neighbor)

        components = 0
        for i in range(n):
            if not visited[i]:
                bfs(i)
                components += 1

        return components - 1
