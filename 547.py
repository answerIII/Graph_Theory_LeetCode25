class Solution(object):
    def findCircleNum(self, isConnected):
        n = len(isConnected)
        count = 0
        visited = bytearray(n)
        
        for i in range(n):
            if not visited[i]:
                count += 1
                visited[i] = 1
                queue = [i]
                while queue:
                    city = queue.pop()
                    for neighbor in range(n):
                        if isConnected[city][neighbor] and not visited[neighbor]:
                            queue.append(neighbor)
                            visited[neighbor] = 1
        return count
    
"""
isConnected = [[1, 1, 0], [1, 1, 0], [0, 0, 1]]
s = Solution()
res = s.findCircleNum(isConnected)
print(res)
"""