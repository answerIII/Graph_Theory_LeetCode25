class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        n = len(graph)
        target = n - 1
        result = []

        node_stack = [0]
        path_stack = [[0]]
        
        while node_stack:
            node = node_stack.pop()
            path = path_stack.pop()
            
            if node == target:
                result.append(path)
                continue
                
            for neighbor in reversed(graph[node]):
                node_stack.append(neighbor)
                path_stack.append(path + [neighbor])
                
        return result

"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""