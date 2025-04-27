from typing import List
from collections import deque, defaultdict


class Solution:
    def findCircleNum(self, isConnected: List[List[int]]) -> int:
        answer = []
        for i in isConnected:
            answer.append(i.count(1))
        print(answer)
        if len(list(set(answer))) == 1:
            return len(answer)
        else:
            return len(list(set(answer)))


sol = Solution()
isConnected = [[1, 1, 0], [1, 1, 0], [0, 0, 1]]
print(sol.findCircleNum(isConnected))

sol = Solution()
isConnected = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
print(sol.findCircleNum(isConnected))
