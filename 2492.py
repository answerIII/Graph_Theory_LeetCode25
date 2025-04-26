class Solution(object):
    def minScore(self, n, roads):
        graph = [[] for _ in range(n + 1)]
        for a, b, distance in roads:
            graph[a].append((b, distance))
            graph[b].append((a, distance))

        min_score = float('inf')
        visited = [False] * (n + 1)
        queue = deque()
        queue.append(1)
        visited[1] = True

        while queue:
            city = queue.popleft()
            for neighbor, distance in graph[city]:
                if distance < min_score:
                    min_score = distance
                if not visited[neighbor]:
                    visited[neighbor] = True
                    queue.append(neighbor)

        return min_score

