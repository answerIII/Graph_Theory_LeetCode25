class Solution(object):
    def allPathsSourceTarget(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: List[List[int]]
        """
        n = len(graph)
        target = n - 1
        paths = []

        def DFS(node, path):
            path.append(node)

            if node == target:
                paths.append(path[:])
            else:
                for neighbor in graph[node]:
                    DFS(neighbor,path)

            path.pop()

        DFS(0,[])
        return paths

"""
graph = [[4,3,1],[3,2,4],[3],[4],[]]
solution = Solution()
result = solution.allPathsSourceTarget(graph)
print(result)
"""