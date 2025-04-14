from typing import List

class Solution:
    def DFS(self, n, graph, visited):
        stack = [0]
        visited[0] = True
        while stack != []:
            node = stack.pop()
            for neighbor in graph[node]:
                if graph[node][neighbor] == 1 and visited[neighbor] == False:
                    visited[neighbor] = True
                    stack.append(neighbor)

    def maxNumEdgesToRemove(self, n: int, edges: List[List[int]]) -> int:
        graphAlice = [[] for _ in range(n)]
        graphBob = [[] for _ in range(n)]
        
        for edge in edges:
            edge_type, u, v = edge
            u -= 1 #TODO перевроверить индексы
            v -= 1
            if edge_type == 1:
                graphAlice[u].append(v)
                graphAlice[v].append(u)
            elif edge_type == 2:
                graphBob[u].append(v)
                graphBob[v].append(u)
            elif edge_type == 3:
                graphAlice[u].append(v)
                graphAlice[v].append(u)
                graphAlice[u].append(v)
                graphAlice[v].append(u)

        visitedAlice = [False] * n
        visitedBob = [False] * n
        self.DFS(n, graphAlice, visitedAlice)
        self.DFS(n, graphBob, visitedBob)

        if not all(visitedAlice) or not all(visitedBob):
            return -1

        edgesToRemove = 0
        for edge in edges:
            edge_type, u, v = edge
            u -= 1
            v -= 1

            if edge_type == 1:
                graphAlice[u].remove(v)
                graphAlice[v].remove(u)
                visitedAlice = [False] * n
                self.DFS(n, graphAlice, visitedAlice)
                if all(visitedAlice):
                    edgesToRemove += 1
                else:
                    graphAlice[u].append(v)
                    graphAlice[u].append(v)

            elif edge_type == 2:
                graphBob[u].remove(v)
                graphBob[v].remove(u)
                visitedBob = [False] * n
                self.DFS(n, graphBob, visitedBob)
                if all(visitedBob):
                    edgesToRemove += 1
                else:
                    graphBob[u].append(v)
                    graphBob[v].append(u)

            elif edge_type == 3:
                graphAlice[u].remove(v)
                graphAlice[v].remove(u)
                graphBob[u].remove(v)
                graphBob[v].remove(u)
                visitedAlice = [False] * n
                visitedBob = [False] * n
                self.DFS(n, graphAlice, visitedAlice)
                self.DFS(n, graphBob, visitedBob)
                if all(visitedAlice) and all(visitedBob):
                    edgesToRemove += 1
                else:
                    graphAlice[u].append(v)
                    graphAlice[v].append(u)
                    graphBob[u].append(v)
                    graphBob[v].append(u)

        return edgesToRemove
