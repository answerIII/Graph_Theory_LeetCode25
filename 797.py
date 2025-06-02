class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        n = len(graph)
        target = n - 1
        result = []
        
        path = [0] * n
        path_size = [1]
        
        def dfs(node):
            if node == target:
                result.append(path[:path_size[0]])
                return
            
            neighbors = graph[node]
            current_size = path_size[0]
            
            for neighbor in neighbors:
                path[current_size] = neighbor
                path_size[0] = current_size + 1
                dfs(neighbor)
            
            path_size[0] = current_size

        dfs(0)
        return result
    
"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""