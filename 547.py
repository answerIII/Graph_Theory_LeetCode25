class Solution(object):
    def findCircleNum(self, isConnected):
        """
        :type isConnected: List[List[int]]
        :rtype: int
        """
        n = len(isConnected)
        visited = [False] * n
        count = 0

        def DFS(city):
            visited[city] = True
            for neighbor in range(n):
                if (isConnected[city][neighbor] == 1) and (visited[neighbor] == False):
                    DFS(neighbor)

        for i in range(n):
            if visited[i] == False:
                DFS(i)
                count += 1

        return count


isConnected = [[1, 1, 0], [1, 1, 0], [0, 0, 1]]
s = Solution()
res = s.findCircleNum(isConnected)
print(res)