class Solution(object):
    def findCircleNum(self, isConnected):
        """
        :type isConnected: List[List[int]]
        :rtype: int
        """
        n = len(isConnected)
        count = 0
        visited = [False] * n

        stack = []
        for i in range(n):
            if not visited[i]:
                stack.append(i)
                count += 1
                while stack:
                    city = stack.pop()
                    visited[city] = True
                    for neighbor in range(n):
                        if isConnected[city][neighbor] and not visited[neighbor]:
                            stack.append(neighbor)
                            visited[neighbor] = True
        return count

"""
isConnected = [[1, 1, 0], [1, 1, 0], [0, 0, 1]]
s = Solution()
res = s.findCircleNum(isConnected)
print(res)
"""