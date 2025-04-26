class Solution(object):
    def sumOfDistancesInTree(self, n, edges):
        if n == 1:
            return [0]
        graph = {}
        for u, v in edges:
            if u not in graph:
                graph[u] = []
            if v not in graph:
                graph[v] = []
            graph[u].append(v)
            graph[v].append(u)
        
        count = [1] * n
        answer = [0] * n

        def dfs(node, parent):
            for neighbor in graph[node]:
                if neighbor == parent:
                    continue
                dfs(neighbor, node)
                count[node] += count[neighbor]
                answer[node] += answer[neighbor] + count[neighbor]

        dfs(0, None)

        def dfs2(node, parent):
            for neighbor in graph[node]:
                if neighbor == parent:
                    continue
                answer[neighbor] = answer[node] - count[neighbor] + (n - count[neighbor])
                dfs2(neighbor, node)

        dfs2(0, None)

        return answer
