class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        target = len(graph) - 1

        def dfs(node, path):
            if node == target:
                yield list(path)
                return
            for neighbor in graph[node]:
                path.append(neighbor)
                for p in dfs(neighbor, path):
                    yield p
                path.pop()

        return list(dfs(0, [0]))
    
"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""