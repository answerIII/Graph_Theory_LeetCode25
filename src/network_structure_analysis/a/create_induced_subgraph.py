from typing import (
    List,
    Tuple,
    Set,
    Dict
)


def createInducedSubgraph(undir_adj_list: Tuple[Set[int]],
                          subgraph_nodes_list: List[int]) \
        -> Dict[int, Set[int]]:
    """
    Create an induced subgraph from a given subgraph_nodes_list
    """
    undir_induced_subgraph = {}
    for subgraph_node in subgraph_nodes_list:
        undir_induced_subgraph[subgraph_node] = set()
    for i in range(len(subgraph_nodes_list) - 1):
        node1 = subgraph_nodes_list[i]
        for j in range(i + 1, len(subgraph_nodes_list)):
            node2 = subgraph_nodes_list[j]
            if node2 in undir_adj_list[node1]:
                undir_induced_subgraph[node1].add(node2)
                undir_induced_subgraph[node2].add(node1)
    return undir_induced_subgraph
