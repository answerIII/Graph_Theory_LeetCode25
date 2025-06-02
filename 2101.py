from collections import defaultdict

class Solution(object):
    def maximumDetonation(self, bombs):
        """
        :type bombs: List[List[int]]
        :rtype: int
        """
        n = len(bombs)
        max_bombs = 0
        graph = [[] for _ in range (n)]

        bomb_sqrt = [i[2]**2 for i in bombs]

        for i in range (n):
            x,y,r = bombs[i][0], bombs[i][1], bomb_sqrt[i]
            for j in range (n):
                if i != j:
                    dx = bombs[j][0] - x
                    dy = bombs[j][1] - y
                    if r >= dx**2 + dy**2:
                        graph[i].append(j)

        def DFS(start):
            visited = [False] * n
            stack = [start]
            visited[start] = True
            count = 1
            while stack:
                node = stack.pop()
                for neighbor in graph[node]:
                    if not visited[neighbor]:
                        visited[neighbor] = True
                        stack.append(neighbor)
                        count+=1
            return count
        
        for i in range(n):
            current = DFS(i)
            if current > max_bombs:
                max_bombs = current
                if max_bombs == n:
                    break

        return max_bombs

"""
bombs = [[2,1,3],[6,1,4]]
solution = Solution()
res = solution.maximumDetonation(bombs)
print(res)
"""

"""
bombs = [[1,1,5],[10,10,5]]
solution = Solution()
res = solution.maximumDetonation(bombs)
print(res)
"""

"""
bombs = [[1,2,3],[2,3,1],[3,4,2],[4,5,3],[5,6,4]]
solution = Solution()
res = solution.maximumDetonation(bombs)
print(res)
"""

"""
bombs = [[1,1,100],[81,61,60]]
solution = Solution()
res = solution.maximumDetonation(bombs)
print(res)
"""