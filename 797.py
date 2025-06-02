class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        n = len(graph)
        target = n - 1
        result = []
        stack = [(0,[0])]

        while stack:
            node, path = stack.pop()
            if node == target:
                result.append(path)
                continue
            for neighbor in reversed(graph[node]):
                stack.append((neighbor,path + [neighbor]))

        return result

"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""