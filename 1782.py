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
            smaller_vertex = min(a, b)
            bigger_vertex = max(a, b)
            edge = (smaller_vertex, bigger_vertex) # so (1,2) and (2,1) are the same
            shared[edge] += 1

        vertices_by_deg = sorted(range(1, n + 1), key=lambda x: deg[x])

        answers = []
        for query in queries:
            count = 0
            L = 0
            R = n - 1

            while L < R:
                left_vertex = vertices_by_deg[L]
                right_vertex = vertices_by_deg[R]
                if deg[left_vertex] + deg[right_vertex] > query:
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