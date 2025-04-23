from collections import defaultdict

class Solution(object):
    # @staticmethod
    # def incident(a,b, deg, shared):
    #     return deg[a] + deg[b] - shared[(min(a, b), max(a, b))]

    def countPairs(self, n, edges, queries):
        """
        :type n: int
        :type edges: List[List[int]]
        :type queries: List[int]
        :rtype: List[int]
        """

        deg = defaultdict()
        for i in range(1, n+1):
            deg[i] += 0
        shared = defaultdict(lambda: 0)

        for u, v in edges:
            deg[u] += 1
            deg[v] += 1
            first_vertex = min(u, v)
            second_vertex = max(u, v)
            shared[(first_vertex, second_vertex)] += 1 # so (1, 2) and (2, 1) are the same

        deg_sorted = sorted(deg.items(), key=lambda x: x[1]) # sort by degree

        queries_len = len(queries)
        answers = [0] * queries_len
        for i in range(queries_len):
            count = 0
            L = 0
            R = n - 1
            while L < R:
                a = deg_sorted[L][0]
                b = deg_sorted[R][0]
                if deg[a] + deg[b] > queries[i]:
                    count += (R - L)
                    R -= 1
                else:
                    L += 1
                
            # account for shared, too
            for a, b in shared.keys():
                a_b_was_counted_previously = deg[a] + deg[b] > queries[i]
                a_b_with_account_for_shared_is_wrong = deg[a] + deg[b] - shared[(min(a, b), max(a, b))] <= queries[i]
                if a_b_was_counted_previously and a_b_with_account_for_shared_is_wrong:
                    count -= 1
            answers[i] = count
        return answers