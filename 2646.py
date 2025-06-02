from collections import defaultdict, deque

class Solution(object):
    def minimumTotalPrice(self, n, edges, price, trips):
        """
        :type n: int
        :type edges: List[List[int]]
        :type price: List[int]
        :type trips: List[List[int]]
        :rtype: int
        """
        graph = [[] for _ in range(n)]
        for u, v in edges:
            graph[u].append(v)
            graph[v].append(u)

        node_counts = [0] * n

        def find_path(start, end):
            visited = [False] * n
            stack = [(start, [])]
            while stack:
                curr, path = stack.pop()
                if curr == end:
                    for node in path + [end]:
                        node_counts[node] += 1
                    return
                if not visited[curr]:
                    visited[curr] = True
                    for neighbor in graph[curr]:
                        if not visited[neighbor]:
                            stack.append((neighbor, path + [curr]))

        for start, end in trips:
            find_path(start, end)

        dp = [[-1, -1] for _ in range(n)] 

        def dfs(node, parent):
            full = price[node] * node_counts[node]
            half = (price[node] // 2) * node_counts[node]
            for neighbor in graph[node]:
                if neighbor == parent:
                    continue
                f, h = dfs(neighbor, node)
                full += min(f, h)
                half += f 
            dp[node][0] = full
            dp[node][1] = half
            return full, half

        return min(dfs(0, -1))

"""
n = 4
edges = [[0, 1], [1, 2], [1, 3]]
price = [2, 2, 10, 6]
trips = [[0, 3], [2, 1], [2, 3]]
solution = Solution()
result = solution.minimumTotalPrice(n, edges, price, trips)
print(result)
"""