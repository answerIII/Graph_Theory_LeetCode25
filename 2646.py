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
        graph = defaultdict(list)
        for u, v in edges:
            graph[u].append(v)
            graph[v].append(u)

        node_counts = [0] * n
        for start, stop in trips:
            q = deque([(start, [start])])
            while q:
                curr, path = q.popleft()
                if curr == stop:
                    for node in path:
                        node_counts[node] += 1
                    break
                for neighbor in graph[curr]:
                    if neighbor not in path:
                        q.append((neighbor, path + [neighbor]))

        def DFS(node, parent, can_halve):
            non_halve_price = price[node] * node_counts[node]
            halve_price = (price[node] // 2) * node_counts[node]

            for neighbor in graph[node]:
                if neighbor != parent:
                    n_h_p, h_p = DFS(neighbor, node, True)
                    non_halve_price += min(h_p, n_h_p)
                    if can_halve:
                        halve_price += n_h_p
                    else:
                        halve_price = float('inf')
            return non_halve_price, halve_price

        non_halve, halve = DFS(0, -1, True)
        return min(non_halve, halve)

"""
n = 4
edges = [[0, 1], [1, 2], [1, 3]]
price = [2, 2, 10, 6]
trips = [[0, 3], [2, 1], [2, 3]]
solution = Solution()
result = solution.minimumTotalPrice(n, edges, price, trips)
print(result)
"""