class Solution(object):
    def findCircleNum(self, isConnected):
        """
        :type isConnected: List[List[int]]
        :rtype: int
        """

        visited = [False] * len(isConnected)
        result = 0

        for i in range(len(isConnected)):
            if not visited[i]:
                stack = [i]
                while stack:
                    node = stack.pop()
                    if not visited[node]:
                        visited[node] = True
                        for j in range(len(isConnected)):
                            if isConnected[node][j] == 1 and not visited[j]:
                                stack.append(j)
                result += 1

        return result