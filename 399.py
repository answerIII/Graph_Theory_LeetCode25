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
            if B not in graph:
                graph[B] = {}
            graph[A][B] = value
            graph[B][A] = 1./value

        def DFS(start,stop,visited):
            if start == stop:
                return 1.
            visited.add(start)
            for neighbor, value in graph[start].items():
                if neighbor not in visited:
                    result = DFS(neighbor,stop,visited)
                    if result != -1.:
                        return value * result
            return -1.

        results = []

        for C,D in queries:
            if C not in graph or D not in graph:
                results.append(-1.)
            else:
                results.append(DFS(C,D,set()))

        return results

"""
equations = [["a","b"],["b","c"]]
values = [2.0,3.0]
queries = [["a","c"],["b","a"],["a","e"],["a","a"],["x","x"]]
solution = Solution()
result = solution.calcEquation(equations, values, queries)
print(result)
"""