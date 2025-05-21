from typing import List, Tuple, Set


def createInducedSubgraph(
    undir_adj_list: Tuple[Set[int], ...], subgraph_nodes_list: List[int]
) -> Tuple[Set[int], ...]:
    """
    Create an induced subgraph from a given subgraph_nodes_list
    Returns a subgraph with renamed vertices
    """
    undir_induced_subgraph: Tuple[Set[int], ...] = tuple(
        set() for _ in range(len(subgraph_nodes_list))
    )
    subgraph_nodes_list.sort()
    for i in range(len(subgraph_nodes_list) - 1):
        node1 = subgraph_nodes_list[i]
        for j in range(i + 1, len(subgraph_nodes_list)):
            node2 = subgraph_nodes_list[j]
            if node2 in undir_adj_list[node1]:
                undir_induced_subgraph[i].add(j)
                undir_induced_subgraph[j].add(i)
    return undir_induced_subgraph
