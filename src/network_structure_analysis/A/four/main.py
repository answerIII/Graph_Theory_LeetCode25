from typing import List, Tuple, Set

from create_adj_list import createUndirAdjList

from definitions import (
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
)

from largest_weak_component import getLargestWeakComponent


def getNeighboursEdgeNum(
    undir_adj_list: Tuple[Set[int], ...], node: int
) -> int:
    neighbours_list: List[int] = sorted(undir_adj_list[node])
    neighbours_list_size = len(neighbours_list)
    neighbour_edges = 0
    for i in range(neighbours_list_size - 1):
        node1 = neighbours_list[i]
        for j in range(i + 1, neighbours_list_size):
            node2 = neighbours_list[j]
            if node2 in undir_adj_list[node1]:
                neighbour_edges += 1
    return neighbour_edges


def getNodeClusterCoeff(
    undir_adj_list: Tuple[Set[int], ...], node: int
) -> float:
    node_degree = len(undir_adj_list[node])
    if node_degree <= 1:
        return 0.0
    neighbour_edges = getNeighboursEdgeNum(undir_adj_list, node)
    return (2 * neighbour_edges) / (node_degree * (node_degree - 1))


def getLWCAvgClusterCoeff(
    undir_adj_list: Tuple[Set[int], ...], largest_weak_component: List[int]
) -> float:
    lwc_size = len(largest_weak_component)
    cluster_coeff_sum: float = 0.0
    for node_ind in largest_weak_component:
        cluster_coeff_sum += getNodeClusterCoeff(undir_adj_list, node_ind)
    return cluster_coeff_sum / lwc_size


def processFile(file_path: str) -> None:
    """
    Process given file
    """
    print(f"Processing: {file_path}")
    undir_adj_list = createUndirAdjList(file_path)
    largest_weak_component = getLargestWeakComponent(undir_adj_list)
    lwc_avg_cluster_coefficient = getLWCAvgClusterCoeff(
        undir_adj_list, largest_weak_component
    )
    print(f"LWC average cluster coefficient: {lwc_avg_cluster_coefficient}")


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)
