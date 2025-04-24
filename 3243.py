'''
Objective: for each query queries[i] = [ui, vi] (adding a new unidirectional edge from city ui to vi), 
           calculate the length of the shortest path from city 0 to city n-1 after processing the first i+1 queries, 
           given the initial chain of roads i to i+1. ans is  an array of shortest path lengths after each query
Idea of solution: after each addition of a new edge from u to v, we update the graph and make BFS from vertex 0 to find the
                  shortest path to vertex n-1. results of the BFS after each query are stored in the array ans, 
                  which is returned as the response.
Complexity: O(n)
'''

from collections import deque

class Solution:
    def shortestDistanceAfterQueries(self, n, queries):

        # initialize the adjacency list for a graph of n cities
        adj = [[] for _ in range(n)]

        # initially, according to the condition, the graph is a chain of the type 0 -> 1 -> 2 .. -> n-1
        # so make this

        for i in range(n - 1):
            adj[i].append(i + 1)  

        ans = []

        # we process each request step by step

        for u, v in queries:

            # add a new edge from u to v to the graph
            
            adj[u].append(v)  

            # initialize an array of distances from city 0 to the rest

            dist = [-1] * n # -1 means the city has not been visited yet
            dist[0] = 0

            # queue for BFS, starting from city 0

            queue = deque([0])

            # BFS for finding the shortest path

            while queue:
                cur = queue.popleft()

                # if we reached city n-1  -> shortest path found -> break

                if cur == n - 1:
                    break  

                # iterate over all neighbors of the current city

                for i in adj[cur]:

                    # if the neighbor has not been visited yet, update the distance and add to the queue

                    if dist[i] == -1:
                        dist[i] = dist[cur] + 1
                        queue.append(i)

            # after BFS is complete, store the distance to city n-1

            ans.append(dist[n - 1])

        return ans