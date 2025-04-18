from typing import List

class Solution:
    def isPrintable(self, targetGrid: List[List[int]]) -> bool:
        m = len(targetGrid)
        n = len(targetGrid[0])
        
        colors = set()
        for i in range(m):
            for j in range(n):
                colors.add(targetGrid[i][j])
        print("Colors: ", colors, "\n")
        borders = {}
        for i in range(m):
            for j in range(n):
                current_color = targetGrid[i][j]
                
                if current_color not in borders:
                    borders[current_color] = [i, j, i, j]
                else:
                    if i < borders[current_color][0]: #минимумы
                        borders[current_color][0] = i  
                    if j < borders[current_color][1]:
                        borders[current_color][1] = j 

                    if i > borders[current_color][2]: #максимумы
                        borders[current_color][2] = i
                    if j > borders[current_color][3]:
                        borders[current_color][3] = j
        print("Borders: ", borders, "\n")
        for color in borders:
            x1, y1, x2, y2 = borders[color]
            print("Color", color, "with borders: (" , x1 , "," , y1,") to (" , x2 , "," , y2 , ")", "\n")
            
            for i in range(x1, x2+1):
                for j in range(y1, y2+1):
                    print("(", i , "," , j , "), value", targetGrid[i][j], "\n")
                    if targetGrid[i][j] < color:
                        return False  #из предположения, что с каждым разом мы увеличиваем значение цвета
        return True
