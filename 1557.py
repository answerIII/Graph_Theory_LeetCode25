# The previous solution was also too slow
# This is the only valid one

class Solution(object):
    def findSmallestSetOfVertices(self, n, edges):
        in_degree = [0 for _ in range(n)]
        for edge in edges:
            in_degree[edge[1]] += 1

        ans = list()
        for i in range(n):
            if not in_degree[i]:
                ans.append(i)

        return ans
