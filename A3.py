from basic import *
from collections import defaultdict
import itertools

def count_triangles(graph: dict) -> int:
    """Count the number of triangles in an undirected graph."""
    triangles = 0
    for u in graph:
        neighbors_u = set(graph[u])
        for v in neighbors_u:
            if v > u:  # Avoid counting same triangle multiple times
                common_neighbors = neighbors_u & set(graph.get(v, set()))
                triangles += len(common_neighbors)
    return triangles // 3

def average_clustering(graph: dict) -> float:
    """Calculate the average clustering coefficient for the graph."""
    total = 0.0
    count = 0
    for node in graph:
        neighbors = list(graph.get(node, set()))
        degree = len(neighbors)
        if degree < 2:
            continue
        
        possible_edges = degree * (degree - 1) / 2
        actual_edges = 0
        
        for i, j in itertools.combinations(neighbors, 2):
            if j in graph.get(i, set()):
                actual_edges += 1
        
        total += actual_edges / possible_edges
        count += 1
    
    return total / count if count > 0 else 0.0

def global_clustering(graph: dict) -> float:
    """Calculate the global clustering coefficient (transitivity)."""
    triangles = count_triangles(graph)
    triples = 0
    
    for node in graph:
        degree = len(graph.get(node, set()))
        triples += degree * (degree - 1) / 2
    
    return (3 * triangles) / triples if triples > 0 else 0.0