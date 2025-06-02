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

        # Оптимизированный подсчет посещений узлов
        node_counts = [0] * n
        for start, end in trips:
            # BFS с отслеживанием пути через parent-указатели
            parent = [-1] * n
            q = deque([start])
            parent[start] = start  # Маркер для корня
            
            while q:
                curr = q.popleft()
                if curr == end:
                    break
                for neighbor in graph[curr]:
                    if parent[neighbor] == -1:
                        parent[neighbor] = curr
                        q.append(neighbor)
            
            # Восстанавливаем путь и считаем узлы
            node = end
            while node != parent[node]:
                node_counts[node] += 1
                node = parent[node]
            node_counts[start] += 1  # Не забываем стартовый узел

        # Оптимизированная DFS с мемоизацией
        memo = {}
        def dfs(node, parent, can_halve):
            key = (node, can_halve)
            if key in memo:
                return memo[key]
            
            full_price = price[node] * node_counts[node]
            half_price = (price[node] // 2) * node_counts[node] if can_halve else float('inf')
            
            for neighbor in graph[node]:
                if neighbor != parent:
                    neighbor_full, neighbor_half = dfs(neighbor, node, True)
                    full_price += min(neighbor_full, neighbor_half)
                    if can_halve:
                        half_price += dfs(neighbor, node, False)[0]
            
            memo[key] = (full_price, half_price)
            return memo[key]

        full, half = dfs(0, -1, True)
        return min(full, half)

"""
n = 4
edges = [[0, 1], [1, 2], [1, 3]]
price = [2, 2, 10, 6]
trips = [[0, 3], [2, 1], [2, 3]]
solution = Solution()
result = solution.minimumTotalPrice(n, edges, price, trips)
print(result)
"""