#Так, я не знаю куда ответы на вопросы вставлять, так что пока что сюда вставлю
#В общем, дфсом находим количество компонент связанности, сколько компонент столько и провинций
class Solution:
    def findCircleNum(self, isConnected: List[List[int]]) -> int:
        n=len(isConnected)
        v=[False for _ in range(n)]
        otvet=0
        i=0
        def dfs(g):
            s=0
            while s<n:
                if isConnected[g][s]==1 and not v[s]:
                    v[s]=True
                    dfs(s)
                s=s+1
        while i<n:
            if not v[i]:
                otvet=otvet+1
                v[i]=True
                dfs(i)
            i=i+1
        return otvet