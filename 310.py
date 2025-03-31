'''
Objective: to find the root\roots at which the length of the tree will be minimal.
Idea of solution: these roots are in the middle of the longest path of the tree. 
                  Then to find them we will gradually "trim the tree", that is, 
                  gradually remove the leaves of the trees until the central nodes
                  remain.
Complexity: O(n)                  
'''

from collections import deque

class Solution:
    def findMinHeightTrees(self, n, edges):

        # degenerate case when the tree is of length 1
        if n == 1:
            return [0]
        

        #initialize the required arrays

        degree = [0] * n # array of node degrees (how many edges come out of it).
        adj = [[] for _ in range(n)] #adjacency list (for each node stores its neighbors).

        #go through the vertices and count the degrees and neighbors for each vertex.

        for u, v in edges:
            degree[u] += 1
            degree[v] += 1
            adj[u].append(v)
            adj[v].append(u)

        #collect all leaves (nodes with degree = 1) into a deque.

        leaves = deque()
        for i in range(n):  
            if degree[i] == 1:
                leaves.append(i)

        #iteratively remove leaves. 
        # for each neighbor of a leaf we decrease the degree. 
        # check if after removal the degree == 1 -> it becomes a leaf -> add to the dequeue. 
        # repeat until there are 1 or 2 nodes left        

        while n > 2:
            count = len(leaves)
            n -= count

            for _ in range(count):
                leaf = leaves.popleft()
                for neighbor in adj[leaf]:
                    degree[neighbor] -= 1
                    if degree[neighbor] == 1:  
                        leaves.append(neighbor)
        
        return list(leaves)