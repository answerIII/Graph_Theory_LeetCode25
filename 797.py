class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        target = len(graph) - 1
        result = []

        def dfs(node, path):
            if node == target:
                # Копируем только в случае успеха
                result.append(list(path))
                return
            for neighbor in graph[node]:
                path.append(neighbor)
                dfs(neighbor, path)
                path.pop()

        dfs(0, [0])
        return result
    
"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""