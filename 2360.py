'''
Objective: have to find the length of the longest cycle in a directed graph, where each vertex has at most one outgoing edge. 
Idea of solution: goes through each vertex of the graph (DFS), tracking the current path, 
                  and if a cycle is detected (the vertex occurs repeatedly in the path), 
                  calculates its length, updating the maximum. if cycles are found, the length of the longest is returned else -> -1
                  (this idea was discussed in practice lessons)
Complexity: O(n)
'''

class Solution:
    def longestCycle(self, edge):

        # initialize the required arrays and variable

        n = len(edge) # num of vertices in the graph
        visited = [False] * n  #tracking visited vertices (initially all False).
        ans = -1  # answer. no cycle -> -1

        # DFS: iterate over all vertices, if the node vertex has not been visited yet, 
        # we start processing it

        for node in range(n):

            if not visited[node]:

                way = {} # recording steps along the path from a node (the key is the vertex, the value is the step at which it was visited)
                cur = node
                step = 0

                while cur > -1:
                    # check for cycle. if cur is already in the way -> found a cycle.
                    if cur in way:

                        # cycle length is calculated as the difference between the current step 
                        # and the step where the cur was first visited (way[car]).
                        cycle_lenght = step - way[cur]
                        ans = max(cycle_lenght, ans)
                        break

                    # if cur has already been visited before -> it means that we have entered the already
                    # traversed part of the graph, and there are no cycles here 

                    if visited[cur]:
                        break
                    
                    # mark vertex as visited. writing the current step for the cur vertex to way. 
                    # ++step -> go to the next vertex

                    visited[cur] = True
                    way[cur] = step
                    step += 1
                    cur = edge[cur]
        
        if ans > 0:
            return ans
        return -1     
