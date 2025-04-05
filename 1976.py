class Solution(object):
    def countPaths(self, n, roads):
        """
        :type n: int
        :type roads: List[List[int]]
        :rtype: int
        """
        weights = [float('inf')] * n
        weights[0] = 0
        
        ways = [0] * n
        ways[0] = 1

        visited = [False] * n

        heap = [(0, 0)]
        while heap:
            _, min_node = heapq.heappop(heap)
            
            if visited[min_node]:
                continue

            visited[min_node] = True

            for road in roads:
                u, v, time = road

                if u == min_node and not visited[v]:

                    if weights[v] > weights[u] + time:
                        weights[v] = weights[u] + time
                        ways[v] = ways[u]
                        heapq.heappush(heap, (weights[v], v))

                    elif weights[v] == weights[u] + time:
                        ways[v] = ways[v] + ways[u]

                elif v == min_node and not visited[u]:

                    if weights[u] > weights[v] + time:
                        weights[u] = weights[v] + time
                        ways[u] = ways[v]
                        heapq.heappush(heap, (weights[u], u))

                    elif weights[u] == weights[v] + time:
                        ways[u] = ways[u] + ways[v]

        return ways[n - 1] % (10**9 + 7)