from A1 import weekly_connected_components

# На вход получает неориентированный граф
def local_clustering_coefficient(G: dict[int, set[int]], node: int) -> float:
    neighbors = G.get(node, set())
    k = len(neighbors)
    if k < 2:
        return 0.0

    links = 0
    for u in neighbors:
        for v in neighbors:
            if u < v and v in G.get(u, {}):
                links += 1

    return 2 * links / (k * (k - 1))

# На вход получает неориентированный граф
def average_clustering_coefficient(UG: dict[int, list[int]]) -> float:
    _, largest_component = weekly_connected_components(UG)
    
    subgraph = {}
    for u in largest_component:
        subgraph[u] = set(v for v in UG.get(u, set()))

    total = sum(local_clustering_coefficient(subgraph, node) for node in subgraph)
    return total / len(subgraph)
