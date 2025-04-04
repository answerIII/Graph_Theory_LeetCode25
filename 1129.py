from collections import deque

class Solution(object):
    def shortestAlternatingPaths(self, n, redEdges, blueEdges):
        """
        :type n: int
        :type redEdges: List[List[int]]
        :type blueEdges: List[List[int]]
        :rtype: List[int]
        """
        graph = {}
        for i in range(n):
            graph[i] = {'red': [], 'blue': []}

        for u,v in redEdges:
            graph[u]['red'].append(v)
        for u,v in blueEdges:
            graph[u]['blue'].append(v)

        distance = []
        for _ in range(n):
            distance.append([float('inf')] * 2)
        distance[0][0] = 0
        distance[0][1] = 0

        queue = deque([(0,0),(0,1)])

        while queue:
            node,color = queue.popleft()
            current_dist = distance[node][color]

            next_color = 1 - color

            if color == 1:
                edges = graph[node]['red']
            else:
                edges = graph[node]['blue']

            for neighbor in edges:
                if distance[neighbor][next_color] == float('inf'):
                    distance[neighbor][next_color] = current_dist + 1
                    queue.append((neighbor,next_color))

        res = [min(distance[i][0], distance[i][1]) for i in range(n)]
        result = []
        for x in res:
            if x == float('inf'):
                result.append(-1)
            else:
                result.append(x)
        return result

"""
n = 3
redEdges = [[0,1]]
blueEdges = [[2,1]]
solution = Solution()
result = solution.shortestAlternatingPaths(n, redEdges, blueEdges)
print(result)
"""