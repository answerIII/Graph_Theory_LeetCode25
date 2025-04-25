#Так, я не знаю куда ответы на вопросы вставлять, так что пока что сюда вставлю
#В общем, камни лежащие на плоскости можно представить в виде графа, где камни, лежащие в одном ряду или одном столбце, входят в одну компоненту связанности. Каждую компоненту связанности можно сократить до одного камня.
#В итоге решение такое: находим дфс-ом все компоненты связанности, считаем каждую компоненту за 1 камень. Соответственно, максимально возможное кол-во удалённых камней будет равно всем камням минус количество компонент связанности.

class Solution:
    def removeStones(self, stones: List[List[int]]) -> int:
        n=len(stones)
        k=0
        i=0
        j=0
        list=[[] for _ in range(n)]
        p=[False for _ in range(n)]
        def dfs(stone):
            p[stone]=True
            for neighbor in list[stone]:
                if p[neighbor]==False:
                    dfs(neighbor)
        while i<n:
            j=i+1
            while j<n:
                if stones[i][0]==stones[j][0] or stones[i][1]==stones[j][1]:
                    list[i].append(j)
                    list[j].append(i)
                j=j+1
            i=i+1
        i=0
        while i<n:
            if p[i]==False:
                dfs(i)
                k=k+1
            i=i+1
        i=0

        otvet=n-k
        return otvet
        