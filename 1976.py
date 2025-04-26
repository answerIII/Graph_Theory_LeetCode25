import heapq

class Solution(object):
    def countPaths(self, n, roads):
        graph = [[] for _ in range(n)]
        for u, v, time in roads:
            graph[u].append((v, time))
            graph[v].append((u, time))
        
        weights = [float('inf')] * n
        weights[0] = 0
        
        ways = [0] * n
        ways[0] = 1

        visited = [False] * n

        heap = [(0, 0)]
        while heap:
            weight, min_node = heapq.heappop(heap)
            
            if visited[min_node]:
                continue

            visited[min_node] = True

            for neighbor, time in graph[min_node]:
                if visited[neighbor]:
                    continue

                if weights[neighbor] > weights[min_node] + time:
                    weights[neighbor] = weights[min_node] + time
                    ways[neighbor] = ways[min_node]
                    heapq.heappush(heap, (weights[neighbor], neighbor))

                elif weights[neighbor] == weights[min_node] + time:
                    ways[neighbor] = (ways[neighbor] + ways[min_node])

        return ways[n - 1] % (10**9 + 7)
