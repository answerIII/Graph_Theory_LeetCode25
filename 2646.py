from collections import deque

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

        def count_path(start, end):
            parent = [-1] * n
            q = deque([start])
            parent[start] = start
            while q:
                curr = q.popleft()
                if curr == end:
                    break
                for neighbor in graph[curr]:
                    if parent[neighbor] == -1:
                        parent[neighbor] = curr
                        q.append(neighbor)
            node = end
            while node != parent[node]:
                node_counts[node] += 1
                node = parent[node]
            node_counts[start] += 1

        for start, end in trips:
            count_path(start, end)

        dp = [[0, 0] for _ in range(n)]
        visited = [False] * n
        stack = [(0, -1, False)]

        while stack:
            node, parent, visited_children = stack.pop()
            if visited_children:
                base = price[node] * node_counts[node]
                half = (price[node] // 2) * node_counts[node]

                total_full = base
                total_half = half

                for neighbor in graph[node]:
                    if neighbor == parent:
                        continue
                    total_full += min(dp[neighbor][0], dp[neighbor][1])
                    total_half += dp[neighbor][0]

                dp[node][0] = total_full
                dp[node][1] = total_half
            else:
                stack.append((node, parent, True))
                for neighbor in graph[node]:
                    if neighbor != parent:
                        stack.append((neighbor, node, False))

        return min(dp[0][0], dp[0][1])

"""
n = 4
edges = [[0, 1], [1, 2], [1, 3]]
price = [2, 2, 10, 6]
trips = [[0, 3], [2, 1], [2, 3]]
solution = Solution()
result = solution.minimumTotalPrice(n, edges, price, trips)
print(result)
"""