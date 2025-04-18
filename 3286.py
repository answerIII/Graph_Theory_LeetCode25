from collections import deque
from typing import List

class Solution:
    def findSafeWalk(self, grid: List[List[int]], health: int) -> bool:
        
        m = len(grid)
        n = len(grid[0])
        visited = [[False] * n for _ in range(m)]
        queue = deque() #BFS
        queue.append((0, 0, health)) #row,col,health
        visited[0][0] = True

        while queue:
            row, col, health = queue.popleft()

            if (row == m - 1) and (col == n - 1):
                if health >= 1:
                    return True

            if (row - 1 >= 0) and (visited[row - 1][col] == False): #вверх
                if grid[row - 1][col] == 1:
                    health -= 1
                if health >= 1:
                    visited[row - 1][col] = True
                    queue.append((row - 1, col, health))

            if (row + 1 < m) and (visited[row + 1][col] == False): #вниз
                if grid[row + 1][col] == 1:
                    health -= 1
                if health >= 1:
                    visited[row + 1][col] = True
                    queue.append((row + 1, col, health))

            if (col - 1 >= 0) and (visited[row][col - 1] == False): #влево
                if grid[row][col - 1] == 1:
                    health -= 1
                if health >= 1:
                    visited[row][col - 1] = True
                    queue.append((row, col - 1, health))

            if (col + 1 < n) and (visited[row][col + 1] == False): #вправо
                if grid[row][col + 1] == 1:
                    health -= 1
                if health >= 1:
                    visited[row][col + 1] = True
                    queue.append((row, col + 1, health))
        return False