from typing import List

class Solution:
    def DFS(self, n, graph, current_city, visited):
        stack = [current_city]
        visited[current_city] = True
        while stack != []:
            node = stack.pop()
            for neighbor in range(n):
                if graph[node][neighbor] == 1 and visited[neighbor] == False:
                    visited[neighbor] = True
                    stack.append(neighbor)

    def findCircleNum(self, isConnected: List[List[int]]) -> int:
        n = len(isConnected[0])
        visited = [False] * n
        circle_num = 0
        for current_city in range(n):
            if visited[current_city] == False:
                self.DFS(n, isConnected, current_city, visited)
                circle_num += 1
        return circle_num
    
