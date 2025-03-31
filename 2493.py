'''
Objective: divide all vertices into groups with numbers so that each vertex is in exactly one group 
           and if vertices are connected by an edge, their groups must differ by exactly 1. 
           find the maximum number of this division.
           (this idea was partitly discussed in our lectures)
Idea of solution: check if the graph is bipartite (does not contain cycles of odd length) - 
                  otherwise division is impossible.divide the graph into connected components. 
                  for each connected component, find its longest shortest path length.("diametr")
                  sum the diametres of all components and the number of components 
                  (since the numbering of groups starts over for each component)
Complexity: ~= O(n^2)  
'''



from collections import deque

class Solution:
    def magnificentSets(self, n, edges):

        # initialize the graph using the adjacency list
        graph = [[] for _ in range(n + 1)]
        for u, v in edges:
            graph[u].append(v)
            graph[v].append(u)
        

        # select connectivity components. 
        # for this, we use the bfs algorithm implemented using a dequeue. 
        # each component is stored as a set of nodes in the array "components"

        visited = [False] * (n + 1)
        components = []
        
        for node in range(1, n + 1):
            if not visited[node]:
                queue = deque([node])
                visited[node] = True
                component = set()
                component.add(node)
                while queue:
                    u = queue.popleft()
                    for v in graph[u]:
                        if not visited[v]:
                            visited[v] = True
                            component.add(v)
                            queue.append(v)
                components.append(component)
        
        total = 0

        # processing each connectivity component: will select each one as a starting one
        
        for component in components:

            # for each connectivity component look for the maximum number of groups (max_groups)
            # into which it can be divided.

            max_groups = 0

            for start in component:

                groups = [0] * (n + 1)
                groups[start] = 1
                queue = deque([start])
                curr_max = 1
                valid = True
                
                # starting bfs from start component

                while queue and valid:

                    u = queue.popleft()


                    for v in graph[u]:

                        # if the vertex v has not yet been visited, then assign it a group 1 larger than the current vertex 
                        # and update current_max. add to the queue for further traversal

                        if groups[v] == 0:
                            groups[v] = groups[u] + 1
                            curr_max = max(curr_max, groups[v])
                            queue.append(v)

                        # elst the node has already been visited, check the difference between the groups.
                        # if the difference is not 1, splitting is not possible (valid = False) -> end of while

                        elif abs(groups[v] - groups[u]) != 1:
                            valid = False
                            break
                
                # if everything is ok (the division is valid) then we have to check is current maximum is 
                # bigger than previous one 
                # else -> division is not valid -> return -1 (it means that graph is not bipartite)

                if valid:
                    if curr_max > max_groups:
                        max_groups = curr_max
                else:
                    return -1
            
            # if no valid partitions are found for a component -> return -1
            # else add the maximum number of groups for this component to the overall result
            
            if max_groups == 0:
                return -1
            total += max_groups
        
        return total