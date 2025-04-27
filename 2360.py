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

        visited = [False] * n
        answer = -1

        for i in range(n):
            if not visited[i]:
                current = i
                step = {}
                depth = 0

                while current != -1:
                    if current in step:
                        answer = max(answer, depth - step[current])
                        break
                    if visited[current]:
                        break

                    step[current] = depth
                    visited[current] = True

                    if graph[current]:
                        current = graph[current][0]
                    else:
                        current = -1
                    depth += 1

        return answer


sol = Solution()
edges = [3, 3, 4, 2, 3]
print(sol.longestCycle(edges))

sol = Solution()
edges = [2, -1, 3, 1]
print(sol.longestCycle(edges))
