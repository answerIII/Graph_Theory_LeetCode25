from typing import List
from collections import deque, defaultdict


class Solution:
    def findCircleNum(self, isConnected: List[List[int]]) -> int:
        n = len(isConnected)
        visited = [False] * n
        answer = 0

        for city in range(n):
            if not visited[city]:
                queue = deque([city])
                visited[city] = True

                while queue:
                    current = queue.popleft()
                    for neighbor in range(n):
                        if isConnected[current][neighbor] == 1 and not visited[neighbor]:
                            visited[neighbor] = True
                            queue.append(neighbor)

                answer += 1

        return answer


sol = Solution()
isConnected = [[1, 1, 0], [1, 1, 0], [0, 0, 1]]
print(sol.findCircleNum(isConnected))

sol = Solution()
isConnected = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
print(sol.findCircleNum(isConnected))

sol = Solution()
isConnected = [[1, 0, 0, 1], [0, 1, 1, 0], [0, 1, 1, 1], [1, 0, 1, 1]]
print(sol.findCircleNum(isConnected))

sol = Solution()
isConnected = [[1,1,1],[1,1,1],[1,1,1]]
print(sol.findCircleNum(isConnected))
