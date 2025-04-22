'''
Objective: determine which edges of a weighted undirected graph belong to at least one shortest path from the initial vertex
           0 to the final vertex n-1.
Idea of solution: 1. сalculate the shortest distances from vertex 0 to all others (dist0) and from vertex n-1 to all others
                    (dist1) using Dijkstra's algorithm.
                  2. then check each edge: if the sum of the distances from 0 to one end of the edge, the edge weight, 
                  and the distance from the other end to n-1 equals the length of the shortest path, 
                  then the edge is included in some shortest path.
Complexity: O((n + m) log n) m - the number of edges.
'''


import heapq

class Solution:
    def findAnswer(self, n, edges):

        # initialize the graph as an adjacency list
        adj = [[] for _ in range(n)]
        for i, (a, b, w) in enumerate(edges):
            adj[a].append((b, w, i))
            adj[b].append((a, w, i))

        # now find the shortest distances from vertex 0 to all others (dist0)
        
        dist0 = [float('inf')] * n # dist0 is an array where dist0[u] stores the current shortest distance from 0 to u
        dist0[0] = 0
        heap = [(0, 0)] # use min-heap to efficiently select the vertex with the minimum distance

        while heap:

            cur, u = heapq.heappop(heap)

            if cur > dist0[u]:
                continue

            for (v, w, _) in adj[u]:

                # if a shorter path to v via u is found, dist0[v] is updated and the vertex is added to the heap

                if dist0[v] > dist0[u] + w:
                    dist0[v] = dist0[u] + w
                    heapq.heappush(heap, (dist0[v], v))
        

        # then find the shortest distances from vertex n-1 to all others (dist1) (inverse Dijkstra)
        # similar to the previous block, but starts from vertex n-1

        dist1 = [float('inf')] * n # dist1[u] is the shortest distance from u to n-1.
        dist1[n-1] = 0
        heap = [(0, n-1)]

        while heap:

            cur, u = heapq.heappop(heap)

            if cur > dist1[u]:
                continue

            for (v, w, _) in adj[u]:
                if dist1[v] > dist1[u] + w:
                    dist1[v] = dist1[u] + w
                    heapq.heappush(heap, (dist1[v], v))
        

        # now find which edges are included in at least one shortest path from 0 to n-1.

        total = dist0[n - 1] # the length of the shortest path from 0 to n-1
        ans = [False] * len(edges)
        
        if total == float('inf'):
            return ans
        
        for i, (a, b, w) in enumerate(edges):
            
            # an edge is part of a shortest path if the sum of the distances from the start to one end, 
            # the weight of the edge, and the distance from the other end to the finish is equal to the
            # total length of the shortest path

            if (dist0[a] + w + dist1[b] == total) or (dist0[b] + w + dist1[a] == total):
                ans[i] = True
        
        # return an array ans, where ans[i] = True if the edge edges[i] belongs to at least one shortest path.

        return ans