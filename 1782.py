from collections import defaultdict

class Solution(object):
    def countPairs(self, n, edges, queries):
        """
        :type n: int
        :type edges: List[List[int]]
        :type queries: List[int]
        :rtype: List[int]
        """
        deg = [0] * (n + 1)
        shared = defaultdict(int)

        for a, b in edges:
            deg[a] += 1
            deg[b] += 1
            if a > b: # (1, 2) and (2, 1) are the same
                a, b = b, a
            shared[(a, b)] += 1

        sorted_deg = sorted(deg[1:]) # nodes are 1-indexed
        answers = []

        for query in queries:
            count = 0
            L = 0
            R = n - 1

            while L < R:
                if sorted_deg[L] + sorted_deg[R] > query:
                    count += (R - L)
                    R -= 1
                else:
                    L += 1
                
            # account for shared, too
            for (a, b), shared_count in shared.items():
                a_b_was_counted_previously = deg[a] + deg[b] > query
                a_b_with_account_for_shared_is_wrong = deg[a] + deg[b] - shared_count <= query
                
                if a_b_was_counted_previously and a_b_with_account_for_shared_is_wrong:
                    count -= 1

            answers.append(count)
        return answers