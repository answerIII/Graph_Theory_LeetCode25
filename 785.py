class Solution(object):
    def isBipartite(self, graph):
        """
        :type graph: List[List[int]]
        :rtype: bool
        """
        
        adjacency_list = graph
        n = len(graph)
        coloring = [0] * n # 0 for not visited, (-1, 1) for colors

