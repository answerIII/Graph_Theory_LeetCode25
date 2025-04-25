#Так, я не знаю куда ответы на вопросы вставлять, так что пока что сюда вставлю
#В общем, c помощью бфс-а находим самый короткий путь до каждого сервера (для прохода сообщения туда-обратно потребуется в два раз больше времени), находим когда отослали последнее сообщение и складываем - так узнаем когда последнее сообщение дойдёт обртано

class Solution:
    def networkBecomesIdle(self, edges: List[List[int]], patience: List[int]) -> int:
        n=len(patience)
        i=1
        otvet=0
        list=[[] for _ in range(n)]
        t=[-1 for _ in range(n)]
        t[0]=0
        for x in edges:
            list[x[0]].append(x[1])
            list[x[1]].append(x[0])
        q=deque([0])
        while q:
            node=q.popleft()
            for child in list[node]:
                if t[child]==-1:
                    t[child]=t[node]+1
                    q.append(child)
        while i<n:
            p=(2*t[i]-1)//patience[i]
            out=p*patience[i]
            lin=2*t[i]+out      
            otvet=max(otvet, lin)
            i=i+1
        return otvet + 1