class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        target = len(graph) - 1
        result = []
        path = [0]
        
        def DFS(node):
            if node == target:
                result.append(path[:])
                return
            
            for neighbor in graph[node]:
                path.append(neighbor)
                DFS(neighbor)
                path.pop()
        
        DFS(0)
        return result
    
"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""