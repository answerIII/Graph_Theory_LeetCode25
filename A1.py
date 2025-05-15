"""Число вершин, число рёбер, плотность (отношение числа рёбер к максимально возможному
числу рёбер), число компонент слабой связности, долю вершин в максимальной по мощности
компоненте слабой связности. Для ориентированных графов определить число компонент
сильной связности и долю вершин графа в наибольшей компоненте сильной связности
компоненте."""

def number_of_vertices(graph: dict):
    """число вершин"""
    return len(graph)

def number_of_edges(graph: dict, directed: bool):
    """число ребер"""
    if not directed:
        return sum(len(neighbors) for neighbors in graph.values()) // 2
    
    return sum(len(neighbors) for neighbors in graph.values())

def weekly_connected_components(graph: dict):
    return 


    
