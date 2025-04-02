'''
Objective: need to build a weighted graph where the variables are nodes and the equations between them
           are edges with weights. for each query find a path between the variables 
           and calculate the product of the edge weights.
Idea of solution: build a weigted graph where variables are related with partition relations, 
                  then for each query use BFS to find a path between the variables by multiplying 
                  the edge weights.
Complexity: ~O(n^2)
'''

from collections import deque

class Solution:
    def calcEquation(self, equations, values, queries):

        # build a graph as a dictionary of dictionaries. top-level keys are nodes,  nested dictionaries 
        # contain neighbors and edge weights. for each equation a/b = val we add:
        # forward edge a → b with weight val, backward edge b → a with weight 1/val

        graph = {} 
        
        for (a, b), val in zip(equations, values):
            if a not in graph:
                graph[a] = {}
            if b not in graph:
                graph[b] = {}
            graph[a][b] = val
            graph[b][a] = 1.0 / val

        # there I decided to use def for solution: 
        # in fact, this function is a slightly redesigned BFS.
         
        def evaluate_query(c, d):

            # checks whether variables exist in the graph.
            if c not in graph or d not in graph:
                return -1.0

            # queue for BFS: (current node, accumulated product)
            queue = deque([(c, 1.0)])

            # set visited nodes
            visited = set()
            visited.add(c)

            while queue:
                # extract the node and the current product
                node, cur = queue.popleft()

                # checks the neighbors
                for neighbor, w in graph.get(node, {}).items():

                    # if you have found the target variable -> return final product
                    if neighbor == d:
                        return cur * w
                    
                    # ading a new piece to the queue
                    if neighbor not in visited:
                        visited.add(neighbor)
                        queue.append((neighbor, cur * w))

            return -1.0
        
        #apply the evaluate_query function to each 
        # query and collect the results in a list.
        
        ans = []
        for c, d in queries:
            ans.append(evaluate_query(c, d))
        return ans