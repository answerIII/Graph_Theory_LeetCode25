from collections import defaultdict

class Solution(object):
    @staticmethod
    def incident(a,b, deg, shared):
        return deg[a] + deg[b] - shared[(min(a, b), max(a, b))]

    def countPairs(self, n, edges, queries):
        """
        :type n: int
        :type edges: List[List[int]]
        :type queries: List[int]
        :rtype: List[int]
        """

        deg = defaultdict(lambda: 0)
        shared = defaultdict(lambda: 0)

        for u, v in edges:
            deg[u] += 1
            deg[v] += 1
            first_vertex = min(u, v)
            second_vertex = max(u, v)
            shared[(first_vertex, second_vertex)] += 1 # so (1, 2) and (2, 1) are the same

        queries_len = len(queries)
        answers = [0] * queries_len
        for i in range(queries_len):
            count = 0
            for u in range(1, n+1):
                for v in range(u+1, n+1):
                    if self.incident(u, v, deg, shared) > queries[i]:
                        count += 1
            answers[i] = count

        return answers