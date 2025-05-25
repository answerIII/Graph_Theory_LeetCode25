from basic import *
def number_of_vertices(graph: dict) -> int:
    """число вершин"""
    return len(graph)

def number_of_edges(graph: dict, directed: bool) -> int:
    """число ребер"""
    if not directed:
        return sum(len(neighbors) for neighbors in graph.values()) // 2
    
    return sum(len(neighbors) for neighbors in graph.values())


def density(num_of_edges: int, num_of_vertices: int, directed) -> float:
    """плотность"""
    if directed: return num_of_edges / (num_of_vertices * (num_of_vertices - 1)) 
    return  2 * num_of_edges / (num_of_vertices * (num_of_vertices - 1)) 
 
def weekly_connected_components(graph: dict) -> tuple[int, set]:
    """число компонент слабой связности и максимальная по количеству вершин комопнента"""
    visited = set()
    components = []
    count  = 0
    for v in graph:
        if v not in visited:
            components.append(dfs_iterative_with_component(graph, visited, v))
            count+=1
    max_component = max(components, key=len)  # компонентa с макс. числом вершин
    return count, max_component

                
def kosarju(graph: dict):
    """кол-во компонент сильной связности, исп. Косарайю"""
    visited = set()
    posled = []
    count = 0
    components = []
    for v in graph:
        if v not in visited:
            dfs_iterative_with_time_out(graph, visited, v, posled)
    visited.clear()
    reversed_graph = reverse_graph(graph)
    for v in reversed(posled):
        if v not in visited:
            count+=1
            components.append( dfs(reversed_graph,visited, v))
    max_component_count = len(max(components, key=len))  # компонентa с макс. числом вершин

    return count, max_component_count
            
def strongly_connected_components(graph: dict):
    """для орграфов определить число компонент сильной связности"""
    count, max_comp = kosarju(graph)
    return count, max_comp
