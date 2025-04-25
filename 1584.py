#Так, я не знаю куда ответы на вопросы вставлять, так что пока что сюда вставлю
#В общем, расчитываем расстояние от каждой точки до каждой, и потом по алгоритму Прима выбираем минимальные расстояния до ещё не посещённых точек пока не построим мнимальное остовное дерево
class Solution:
    def minCostConnectPoints(self, points: List[List[int]]) -> int:
        n = len(points)
        h = []
        v = {0}
        i=1
        e=0
        otvet=0
        while i<n:
            c = abs(points[0][0] - points[i][0]) + abs(points[0][1] - points[i][1])
            heapq.heappush(h, (c, i))
            i=i+1
        while h and e < n - 1:
            c, p = heapq.heappop(h)
            if p not in v:
                v.add(p)
                otvet=otvet+c
                e=e+1
                for i in range(n):
                    if i not in v:
                        w = abs(points[p][0] - points[i][0]) + abs(points[p][1] - points[i][1])
                        heapq.heappush(h, (w, i))
        return otvet