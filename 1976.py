import heapq

class Solution(object):
    def countPaths(self, n, roads):

        graph = {}
        for u, v, time in roads:
            if u not in graph:
                graph[u] = []
            if v not in graph:
                graph[v] = []
            graph[u].append((v, time))
            graph[v].append((u, time))
        
        weights = [float('inf')] * n
        weights[0] = 0
        
        ways = [0] * n
        ways[0] = 1

        heap = [(0, 0)]
        while heap:
            weight, min_node = heapq.heappop(heap)

            if weight > weights[min_node]:
                continue

            for neighbor, time in graph.get(min_node, []):

                if weights[neighbor] > weights[min_node] + time:
                    weights[neighbor] = weights[min_node] + time
                    ways[neighbor] = ways[min_node]
                    heapq.heappush(heap, (weights[neighbor], neighbor))

                elif weights[neighbor] == weights[min_node] + time:
                    ways[neighbor] = (ways[neighbor] + ways[min_node]) % (10**9 + 7)

        return ways[n - 1]
