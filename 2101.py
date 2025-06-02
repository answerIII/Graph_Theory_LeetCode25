class Solution(object):
    def maximumDetonation(self, bombs):
        """
        :type bombs: List[List[int]]
        :rtype: int
        """
        n = len(bombs)
        result = [[0,0,0]] * n
        count = 1
        for i in range(n):
            for j in range(2):
                if i+1 < n:
                    result[i][j] = bombs[i][j] - bombs[i+1][j]
                    if result[i][j] >= bombs[i][2] - bombs[i+1][2]:
                        if count < n:
                            count += 1
                else:
                    break
        print(result)
        return count

"""
bombs = [[2,1,3],[6,1,4]]
solution = Solution()
result = solution.maximumDetonation(bombs)
print(result)
"""

"""
bombs = [[1,1,5],[10,10,5]]
solution = Solution()
result = solution.maximumDetonation(bombs)
print(result)
"""

"""
bombs = [[1,2,3],[2,3,1],[3,4,2],[4,5,3],[5,6,4]]
solution = Solution()
result = solution.maximumDetonation(bombs)
print(result)
"""