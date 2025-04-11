from typing import List

class Solution:
    def DFS(self, сurrent_node, graph, state) -> bool:
            stack = [сurrent_node]
            state[сurrent_node] = 0
            while stack != []:
                node = stack.pop()
                for neighbor in graph[node]:
                        if state[neighbor] is None:
                            state[neighbor] = 1 - state[node]
                            stack.append(neighbor)
                        elif state[neighbor] == state[node]:
                            return False
            return True

    def possibleBipartition(self, n: int, dislikes: List[List[int]]) -> bool:
        graph = [[] for _ in range(n)]
        for a, b in dislikes:
            graph[a - 1].append(b - 1)
            graph[b - 1].append(a - 1)
        state = [None] * n
        for сurrent_node in range(n):
            if state[сurrent_node] is None:
                if self.DFS(сurrent_node, graph, state) == False:
                    return False
        return True
