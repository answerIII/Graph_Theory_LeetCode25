from collections import defaultdict

class Solution(object):
    def maximumDetonation(self, bombs):
        """
        :type bombs: List[List[int]]
        :rtype: int
        """
        n = len(bombs)
        result = 0
        graph = defaultdict(list)
        for i in range (n):
            for j in range (n):
                if i != j:
                    if bombs[i][2] ** 2 >= (bombs[i][0] - bombs[j][0]) ** 2 + (bombs[i][1] - bombs[j][1]) ** 2:
                        graph[i] += [j]
        """
        def DFS(node,visited):
            for child in graph[node].items():
                visited.add(child)
                DFS(child,visited)
        """
        def DFS(node,visited):
            visited.add(node)
            for neighbor in graph[node]:
                if neighbor not in visited:
                    result = DFS(neighbor,visited)
                    if result != -1.:
                        return result
            return -1.

        for i in range(n):
            visited = set([i])
            DFS(i,visited)
            result = max(result, len(visited))

        return result


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