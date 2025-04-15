class Solution(object):
    def calcEquation(self, equations, values, queries):
        graph = dict()

        for i in range(len(equations)):
            A = equations[i][0]
            B = equations[i][1]
            element = values[i]

            if A not in graph:
                graph[A] = dict()
            if B not in graph:
                graph[B] = dict()
            
            graph[A][B] = element
            graph[B][A] = 1 / element

        results = []

        for (i, j) in queries:
            if i not in graph or j not in graph:
                results.append(-1)
                continue
            if i == j:
                results.append(1)
                continue

            visited = set()
            stack = [(i, 1)]
            found = False

            while stack:
                node, value = stack.pop()
                if node == j:
                    results.append(value)
                    found = True
                    break
                visited.add(node)
                for key in graph[node]:
                    if key not in visited:
                        stack.append((key, value * graph[node][key]))

            if not found:
                results.append(-1)

        return results