"""Число вершин, число рёбер, плотность (отношение числа рёбер к максимально возможному
числу рёбер), число компонент слабой связности, долю вершин в максимальной по мощности
компоненте слабой связности. Для ориентированных графов определить число компонент
сильной связности и долю вершин графа в наибольшей компоненте сильной связности
компоненте."""
from basic import *
def number_of_vertices(graph: dict) -> int:
    """число вершин"""
    return len(graph)

def number_of_edges(graph: dict, directed: bool) -> int:
    """число ребер"""
    if not directed:
        return sum(len(neighbors) for neighbors in graph.values()) // 2
    
    return sum(len(neighbors) for neighbors in graph.values())


def density(num_of_edges: int, num_of_vertices: int) -> int:
    """плотность"""
    return num_of_edges / (num_of_vertices * (num_of_vertices - 1)) 
 
def weekly_connected_components(graph: dict):
    visited = set()
    count  = 0
    for v in graph:
        if v not in visited:
            dfs_iterative(graph, visited, v)
            count+=1
            
    return count


    
