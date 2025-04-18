from collections import deque
from typing import List

class Solution:
    def findSafeWalk(self, grid: List[List[int]], health: int) -> bool:

        m = len(grid)
        n = len(grid[0])
        max_health = [[0] * n for _ in range(m)]

        queue = deque() #BFS

        queue.append((0, 0, health)) #row,col,health

        while queue:
            row, col, health = queue.popleft()
            
            if grid[row][col] == 1:
                health -= 1

            if health <= 0 or max_health[row][col] >= health:
                continue
           
            max_health[row][col] = health
            
            if (row == m - 1) and (col == n - 1):
                if health >= 1:
                    return True

            if (row - 1 >= 0): #вверх
                queue.append((row - 1, col, health))
            if (row + 1 < m): #вниз
                queue.append((row + 1, col, health))
            if (col - 1 >= 0): #влево
                queue.append((row, col - 1, health))
            if (col + 1 < n): #вправо
                queue.append((row, col + 1, health))
        
        return False