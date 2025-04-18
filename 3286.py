from collections import deque
from typing import List

class Solution:
    def findSafeWalk(self, grid: List[List[int]], health: int) -> bool:
        
        m = len(grid)
        n = len(grid[0])
        queue = deque() #BFS
        queue.append((0, 0, health - 1 if grid[0][0] == 1 else health)) #row,col,health

        while queue:
            row, col, health = queue.popleft()

            if (row == m - 1) and (col == n - 1):
                if health >= 1:
                    return True

            if (row - 1 >= 0): #вверх
                if grid[row - 1][col] == 1:
                    new_health = health - 1
                else:
                    new_health = health

                if new_health >= 1:
                    queue.append((row - 1, col, new_health))

            if (row + 1 < m): #вниз
                if grid[row + 1][col] == 1:
                    new_health = health - 1
                else:
                    new_health = health
                if  new_health >= 1:
                    queue.append((row + 1, col, new_health))

            if (col - 1 >= 0): #влево
                if grid[row][col - 1] == 1:
                    new_health = health - 1
                else:
                    new_health = health
                if  new_health >= 1:
                    queue.append((row, col - 1, new_health))

            if (col + 1 < n): #вправо
                if grid[row][col + 1] == 1:
                    new_health = health - 1
                else:
                    new_health = health
                if  new_health >= 1:
                    queue.append((row, col + 1, new_health))
        return False