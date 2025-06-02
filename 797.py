class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        result = []
        stack = [(0,)]
        while stack:
            cur_path = stack.pop()
            node = cur_path[-1]
            if node == len(graph) - 1:
                result.append(cur_path)
            for i in graph[node]:
                stack.append(cur_path + (i,))
        return result
    
"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""