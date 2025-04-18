from typing import List
from collections import defaultdict

class Solution:
    def DFS(self, color, visited, graph): #предположение из прошлого решения оказалось неверно, так что используем аналогичное 210 задаче решение
        if visited[color] == 0:
            visited[color] = 1

            for next_color in graph[color]:
                if self.DFS(next_color, visited, graph) == False:
                    return False

            visited[color] = 2
            return True

        if visited[color] == 1:
            return False  #нашли цикл

        if visited[color] == 2:
            return True

    def isPrintable(self, targetGrid: List[List[int]]) -> bool:
        m = len(targetGrid)
        n = len(targetGrid[0])
        
        colors = set()
        for i in range(m):
            for j in range(n):
                colors.add(targetGrid[i][j])
        #print("Colors: ", colors, "\n")

        borders = {}
        for i in range(m):
            for j in range(n):
                current_color = targetGrid[i][j]

                if current_color not in borders:
                    borders[current_color] = [i, j, i, j]
                else:
                    if i < borders[current_color][0]:
                        borders[current_color][0] = i
                    if j < borders[current_color][1]:
                        borders[current_color][1] = j
                    if i > borders[current_color][2]:
                        borders[current_color][2] = i
                    if j > borders[current_color][3]:
                        borders[current_color][3] = j
        #print("Borders: ", borders, "\n")

        graph = defaultdict(list)
        for color in borders:
            x1, y1, x2, y2 = borders[color]
            #print("Color", color, "with borders: (" , x1 , "," , y1,") to (" , x2 , "," , y2 , ")", "\n")

            for i in range(x1, x2 + 1):
                for j in range(y1, y2 + 1):
                    value = targetGrid[i][j]
                    #print("(", i , "," , j , "), value", value, "\n")
                    if value != color:
                        graph[color].append(value)

        visited = defaultdict(int)

        for color in colors:
            if visited[color] == 0:
                if self.DFS(color, visited, graph) == False:
                    return False

        return True