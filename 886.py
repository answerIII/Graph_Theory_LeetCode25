from typing import List

class Solution:
    def DFS(self, сurrent_node, n, graph, visited, color) -> bool:
            stack = [сurrent_node]
            visited[сurrent_node] = True
            color[сurrent_node] = 0
            while stack != []:
                node = stack.pop()
                for neighbor in range(n):
                    if graph[node][neighbor] == 1 and visited[neighbor] == False:
                        if color[neighbor] is None:
                            color[neighbor] = 1 - color[node]
                            visited[neighbor] = True
                            stack.append(neighbor)
                        elif color[neighbor] == color[node]:
                            return False
            return True

    def possibleBipartition(self, n: int, dislikes: List[List[int]]) -> bool:
        graph = [[0] * n for _ in range(n)]
        for a, b in dislikes:
            graph[a - 1][b - 1] = 1
            graph[b - 1][a - 1] = 1
        visited = [False] * n
        color = [None] * n
        for сurrent_node in range(n):
            if visited[сurrent_node] == False:
                if self.DFS(сurrent_node, n, graph, visited, color) == False:
                    return False
        return True
