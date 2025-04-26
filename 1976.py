import heapq

class Solution(object):
    def countPaths(self, n, roads):
        graph = [[] for _ in range(n + 1)]
        for u, v, time in roads:
            graph[u].append((v, time))
            graph[v].append((u, time))

        distance = [float('inf')] * n
        ways = [0] * n
        distance[0] = 0
        ways[0] = 1

        queue = [(0, 0)]

        while queue:
            current_dist, u = heapq.heappop(queue)
            if current_dist > distance[u]:
                continue
            for v, time in graph[u]:
                if distance[v] > distance[u] + time:
                    distance[v] = distance[u] + time
                    ways[v] = ways[u]
                    heapq.heappush(queue, (distance[v], v))
                elif distance[v] == distance[u] + time:
                    ways[v] = (ways[v] + ways[u])
        return ways[n-1] % (10**9 + 7)

