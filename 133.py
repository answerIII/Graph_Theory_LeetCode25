'''
Solution idea: in order to copy the entire graph, go through it with the DFS algorithm
'''

"""
# Definition for a Node.
class Node(object):
    def __init__(self, val = 0, neighbors = None):
        self.val = val
        self.neighbors = neighbors if neighbors is not None else []
"""


class Solution:
    def cloneGraph(self, node):

        #If were given empty graph - must return None

        if not node:
            return None
        
        #The dict for marking visited nodes in graph

        visited = {}

        #Implementation of the DFS algorithm: if a vertex is not visited, 
        #then we mark it as visited and call the algorithm recursively for its neighbors
        
        def dfs(node):
            if node in visited:
                return visited[node]
            
            #To create a copy, we call the class constructor

            copy = Node(node.val)
            visited[node] = copy
            
            for neighbor in node.neighbors:
                copy.neighbors.append(dfs(neighbor))
            
            return copy
        
        return dfs(node)
