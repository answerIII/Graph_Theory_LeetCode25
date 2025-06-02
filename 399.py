from collections import defaultdict

class Solution(object):
    def calcEquation(self, equations, values, queries):
        """
        :type equations: List[List[str]]
        :type values: List[float]
        :type queries: List[List[str]]
        :rtype: List[float]
        """
        graph = {}
        for (A,B), value in zip(equations,values):
            if A not in graph:
                graph[A] = {}
            graph[A][B] = value
            if B not in graph:
                graph[B] = {}
            graph[B][A] = 1./value

        def DFS(start,stop):
            if start not in graph or stop not in graph:
                return -1.
            visited=set()
            s = [(start, 1.)]
            while s:
                neighbor,result = s.pop()
                if neighbor == stop:
                    return result
                visited.add(neighbor)
                for i in graph[neighbor]:
                    if i not in visited:
                        s.append((i,result*graph[neighbor][i]))
            return -1.0

        return [DFS(A,B) for A,B in queries]


equations = [["a","b"],["b","c"]]
values = [2.0,3.0]
queries = [["a","c"],["b","a"],["a","e"],["a","a"],["x","x"]]
solution = Solution()
result = solution.calcEquation(equations, values, queries)
print(result)