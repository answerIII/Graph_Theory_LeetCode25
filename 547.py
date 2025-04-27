from typing import List
from collections import deque, defaultdict


class Solution:
    def findCircleNum(self, isConnected: List[List[int]]) -> int:
        answer = []
        for i in isConnected:
            answer.append(i.count(1))
        answer = len(list(set(answer)))

        return answer


sol = Solution()
isConnected = [[1, 1, 0], [1, 1, 0], [0, 0, 1]]
print(sol.findCircleNum(isConnected))
