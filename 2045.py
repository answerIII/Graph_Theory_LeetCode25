class Solution(object):
    def secondMinimum(self, n, edges, time, change):
        graph = [[] for _ in range(n + 1)]

        for a, b in edges:
            graph[a].append(b)
            graph[b].append(a)

        queue = [(1, 1)]
        first_min = [-1] * (n + 1)
        second_min = [-1] * (n + 1)
        first_min[1] = 0

        while queue:
            x, freq = queue.pop(0)
            t = first_min[x] if freq == 1 else second_min[x]

            if ((t // change) % 2) == 0:
                t += time
            else:
                t = change * (t // change + 1) + time

            for y in graph[x]:
                if first_min[y] == -1:
                    first_min[y] = t
                    queue.append((y, 1))
                elif first_min[y] != t and second_min[y] == -1:
                    if y == n:
                        return t
                    second_min[y] = t
                    queue.append((y, 2))

        return 0
