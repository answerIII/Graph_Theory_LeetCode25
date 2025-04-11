'''
Objective: find the maximum number of nodes in the tree that can be visited, starting from node 0, 
           without entering the restricted nodes.
Idea of solution: use BFS starting at node 0, while skipping all restricted nodes. count all the reachable nodes 
                  that are not on the restricted list and can be reched without restrictions.
Complexity: O(n)
'''

from collections import deque

class Solution:
    def reachableNodes(self, n, edges, restricted):
        restricted = set(restricted) #for fast search
        
        # initialize the graph as an adjacency list
        adj = [[] for _ in range(n)]
        for u, v in edges:
            adj[u].append(v)
            adj[v].append(u)

        visited = [False] * n # an array for marking visited nodes (initially all False)

        # queue for BFS, starting from node 0. mark 0 as visited (visited[0] = True)
        # count is answer - count of reachable nodes

        queue = deque()
        queue.append(0)
        visited[0] = True
        count = 0
        
        # BFS algorithm
        
        while queue:
            
            # while the queue is not empty -> extract the cur node from the beginning of the queue.
            # increasing the count counter, since cur is the new reachable node.

            cur = queue.popleft()
            count += 1
            
            # processing the neighbors of the current node

            for neighbor in adj[cur]:
                
                # if the neighbor is not visited and not restricted -> next cur node for BFS -> mark it as visited 
                # and add it to the queue => visit all accessible nodes, skip the forbidden ones

                if not visited[neighbor] and neighbor not in restricted:
                    visited[neighbor] = True
                    queue.append(neighbor)
        
        return count