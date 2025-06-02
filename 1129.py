from collections import deque

class Solution(object):
    def shortestAlternatingPaths(self, n, redEdges, blueEdges):
        """
        :type n: int
        :type redEdges: List[List[int]]
        :type blueEdges: List[List[int]]
        :rtype: List[int]
        """
        red = [[] for _ in range(n)]
        blue = [[] for _ in range(n)]
    
        for u, v in redEdges:
            red[u].append(v)
        for u, v in blueEdges:
            blue[u].append(v)

        res = [-1] * n
        res[0] = 0
        q = deque([(0, 0), (0, 1)])
        dist = [[-1]*2 for _ in range(n)]
        dist[0][0] = dist[0][1] = 0

        while q:
            node, color = q.popleft()
            edges = blue[node] if color else red[node]
        
            for v in edges:
                if dist[v][1-color] == -1:
                    dist[v][1-color] = dist[node][color] + 1
                    if res[v] == -1 or dist[v][1-color] < res[v]:
                        res[v] = dist[v][1-color]
                    q.append((v, 1-color))
    
        return res

"""
n = 3
redEdges = [[0,1]]
blueEdges = [[2,1]]
solution = Solution()
result = solution.shortestAlternatingPaths(n, redEdges, blueEdges)
print(result)
"""