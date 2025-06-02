class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        target = len(graph) - 1
        result = []
        
        def DFS(node,path):
            if node == target:
                result.append(path)
            for i in graph[node]:
                DFS(i,path+[i])
        
        DFS(0,[0])
        return result
    
"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""