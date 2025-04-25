class Solution(object):
    def minimumFuelCost(self, roads, seats):
        if not roads:
            return 0

        n = len(roads) + 1
        graph = [[] for _ in range(n)]

        for u, v in roads:
            graph[u].append(v)
            graph[v].append(u)

        stack = [(0, -1, False)]
        people = [1] * n
        ans = 0

        while stack:
            u, parent, processed = stack.pop()
            if not processed:
                stack.append((u, parent, True))
                for v in reversed(graph[u]):
                    if v != parent:
                        stack.append((v, u, False))
            else:
                for v in graph[u]:
                    if v != parent:
                        people[u] += people[v]
                if u != 0:
                    ans += (people[u] + seats - 1) // seats
        return ans
