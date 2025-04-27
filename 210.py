from typing import List
from collections import deque, defaultdict


class Solution:
    def findOrder(self, numCourses: int, prerequisites: List[List[int]]) -> List[int]:
        graph = defaultdict(list)
        in_degree = [0] * numCourses

        for u, v in prerequisites:
            graph[v].append(u)
            in_degree[u] += 1

        queue = deque([i for i in range(numCourses) if in_degree[i] == 0])
        result = []

        while queue:
            cur = queue.popleft()
            result.append(cur)
            for neighbor in graph[cur]:
                in_degree[neighbor] -= 1
                if in_degree[neighbor] == 0:
                    queue.append(neighbor)
        if len(result) == numCourses:
            return result
        else:
            return []


numCourses = 3
prerequisites = [[1,0],[1,2],[0,1]]
sol = Solution()
print(sol.findOrder(numCourses, prerequisites))
