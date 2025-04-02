'''
Objective: there is a graph of n nodes and a list of edges times, where each edge is defined as (ui, vi, wi)
           — a signal passes from node ui to node vi in ​​time wi.
           need to send a signal from a given node k and find the minimum time for the signal to reach all other nodes. 
           impossible -> -1
           (this idea of solution was partitly discussed in our lectures)
Idea of solution: use Dijkstra's algorithm to find the shortest paths from node k to all others. 
                  the longest of these paths is the answer.
Complexity: O(E log V) due to using heap
'''

import heapq

class Solution:
    def networkDelayTime(self, times, n, k):

        # build a graph in the form of an adjacency dictionary
        graph = {}

        for u, v, w in times:
            if u not in graph:
                graph[u] = []
            graph[u].append((v, w))

        # initialize distances. all nodes except k have infinite distance (cause signal already in k_th node)
        # distances is a dictionary where keys are node's nums and items are distances

        distances = {node: float('inf') for node in range(1, n + 1)}
        distances[k] = 0

        # one of the quick ways to implement Dijkstra's algorithm
        # is to use a priority queue (we discussed this in the lecture)
        # so I will use heapq from Python and initialize it with start node k with 0

        heap = []
        heapq.heappush(heap, (0, k))

        # implementation of Dijkstra's algorithm: sequentially processes the nodes of the graph, 
        # at each step choosing the node with the smallest known distance 
        # and updating the distances to it's neighbors

        while heap:
            
            #extracting a node from a heap with min cost path
            cur_d, cur_n = heapq.heappop(heap)
            

            # if current_distance > distances[current_node],
            # -> the node has already been processed with a shorter path
            # and the current entry is out of date -> skip it.
            # OR if the node current_node has no outgoing edges -> skip it.

            if cur_d > distances[cur_n] or (cur_n not in graph):
                continue

            # for each neighbor, calculates a new distance. if the new distance is less than the stored one, 
            # then update it in the dictionary. adds (distance, neighbor) to the heap for further consideration.

            for neighbor, w in graph[cur_n]:
                distance = cur_d + w
                if distance < distances[neighbor]:
                    distances[neighbor] = distance
                    heapq.heappush(heap, (distance, neighbor))

        max_distance = max(distances.values())

        if max_distance != float('inf'): 
            return max_distance
        
        return -1