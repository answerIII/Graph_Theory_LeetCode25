from typing import List

class Solution:
    def DFS(self, current_node, graph, state, color) -> bool:
            state[current_node] = color
            for neighbor in graph[current_node]:
                if state[neighbor] == color:
                    return False
                if state[neighbor] is None and self.DFS(neighbor, graph, state, 1 - color) == False:
                    return False
            return True

    def possibleBipartition(self, n: int, dislikes: List[List[int]]) -> bool:
        graph = [[] for _ in range(n)]
        for a, b in dislikes:
            graph[a - 1].append(b - 1)
            graph[b - 1].append(a - 1)
        state = [None] * n
        for current_node in range(n):
            if state[current_node] is None:
                if self.DFS(current_node, graph, state, 0) == False:
                    return False
        return True