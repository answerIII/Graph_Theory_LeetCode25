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

        def DFS(start,stop,visited):
            if start == stop:
                return 1.
            visited.add(start)
            neighbors = graph[start]
            for neighbor in neighbors:
                if neighbor not in visited:
                    res = DFS(neighbor, stop, visited)
                    if res != -1.0:
                        return neighbors[neighbor] * res
            return -1.0

        result = []
        append_result = result.append
        for C, D in queries:
            if C in graph and D in graph:
                append_result(DFS(C, D, set()))
            else:
                append_result(-1.0)
        return result


equations = [["a","b"],["b","c"]]
values = [2.0,3.0]
queries = [["a","c"],["b","a"],["a","e"],["a","a"],["x","x"]]
solution = Solution()
result = solution.calcEquation(equations, values, queries)
print(result)