from typing import List
from collections import deque, defaultdict


class Solution:
    def findCircleNum(self, isConnected: List[List[int]]) -> int:
        answer = 0
        for i in isConnected:
            if i.count(1) == 1:
                answer += 1
        return answer


sol = Solution()
isConnected = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
print(sol.findCircleNum(isConnected))
