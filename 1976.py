import heapq

class Solution(object):
    def countPaths(self, n, roads):

        graph = {i: {} for i in range(n)}
        for u, v, time in roads:
            graph[u][v] = time
            graph[v][u] = time
        
        weights = [float('inf')] * n
        weights[0] = 0
        
        ways = [0] * n
        ways[0] = 1

        heap = [(0, 0)]

        while heap:
            weight, min_node = heapq.heappop(heap)

            if weight > weights[min_node]:
                continue

            for neighbor, time in graph[min_node].items():

                if weights[neighbor] > weights[min_node] + time:
                    weights[neighbor] = weights[min_node] + time
                    ways[neighbor] = ways[min_node]
                    heapq.heappush(heap, (weights[min_node] + time, neighbor))

                elif weights[min_node] + time == weights[neighbor]:
                    ways[neighbor] = (ways[neighbor] + ways[min_node])

        return ways[n - 1] % (10**9 + 7)
