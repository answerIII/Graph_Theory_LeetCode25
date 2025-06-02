class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        n = len(graph)
        target = n - 1
        result = []

        def dfs(node, path):
            if node == target:
                result.append(path + [node])
                return
            
            for neighbor in graph[node]:
                dfs(neighbor, path + [node])

        dfs(0, [])
        return result