from random import (
    choice,
    sample
)

from typing import (
    List,
    Tuple,
    Set
)

from create_adj_list import (
    createUndirAdjList,
)

from definitions import (
    INF,
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR
)

from network_structure_analysis.a.bfs import (
    getWeakComponentBFS,
    getFurthestNodeBFS,
    updateDistNodeSubsetBFS,
    getDistancesListParallelBFS,
    getNodeListWithSnowballBFS
)

from network_structure_analysis.a.create_induced_subgraph import (
    createInducedSubgraph
)


def getLargestWeakComponent(undir_adj_list: Tuple[Set[int]]) -> List[int]:
    visited = list(False for _ in range(len(undir_adj_list)))
    largest_weak_component = []
    for i in range(len(undir_adj_list)):
        new_weak_component = []
        if not visited[i]:
            new_weak_component = getWeakComponentBFS(
                undir_adj_list, visited, i)
        if len(largest_weak_component) < len(new_weak_component):
            largest_weak_component = new_weak_component.copy()
    return largest_weak_component


def displayLWCDiamWithTwoBFS(undir_adj_list: Tuple[Set[int]],
                             largest_weak_component: List[int]) -> None:
    """
    Prints an estimated largest weak component diameter using a double BFS
    """
    ITERATIONS = 5
    lwc_diameter = 0
    for _ in range(ITERATIONS):
        random_node = choice(largest_weak_component)
        furthest_node, distance = getFurthestNodeBFS(
            undir_adj_list, random_node)
        lwc_diameter = max(lwc_diameter, getFurthestNodeBFS(
            undir_adj_list, furthest_node)[1])
    print(f"LWC diameter with two BFS: {lwc_diameter}")


def displayLWCDiamAndPercentWithRandVertDist1(
        undir_adj_list: Tuple[Set[int]],
        largest_weak_component: List[int]) -> None:
    """
    Using selected random nodes and distances between them prints:
        1. Estimted largest weak component diameter
        2. Estimated 90th percentile distance between vertices of LWC
    The BFS launch is used from each of the selected vertices consistently
    """
    ITERATIONS = 1
    RANDOM_NODES_LST = [500]
    percentile_90_sum = 0
    lwc_diameter = 0
    for _ in range(ITERATIONS):
        for nodes_to_select in RANDOM_NODES_LST:
            selected_nodes = sample(
                largest_weak_component, nodes_to_select)
            selected_nodes.sort()
            node_to_ind_map = {}
            for ind, node in enumerate(selected_nodes):
                node_to_ind_map[node] = ind
            distance_matrix = [[INF for j in range(nodes_to_select)]
                               for i in range(nodes_to_select)]
            for node_ind in selected_nodes:
                updateDistNodeSubsetBFS(undir_adj_list, distance_matrix,
                                        node_to_ind_map, node_ind)
            sorted_dist = []
            for i in range(nodes_to_select):
                for j in range(nodes_to_select):
                    if i == j:
                        continue
                    sorted_dist.append(distance_matrix[i][j])
            sorted_dist.sort()
            percentile_90_ind = round(nodes_to_select * 0.9)
            lwc_diameter = max(lwc_diameter, sorted_dist[-1])
            percentile_90_sum += sorted_dist[percentile_90_ind]
    print("LWC diameter with distance with random vertices: "
          f"{lwc_diameter}")
    print("LWC 90th percentile distance with random verices: "
          f"{round(percentile_90_sum / ITERATIONS)}")


def displayLWCDiamAndPercentWithRandVertDist2(
        undir_adj_list: Tuple[Set[int]],
        largest_weak_component: List[int]) -> None:
    """
    Using selected random nodes and distances between them prints:
        1. Estimated largest weak component diameter
        2. Estimated 90th percentile distance between vertices of LWC
    Algorithm uses parallel launch of BFS from selected vertices
    Note: Unusable due to the huge amount of RAM usage
    """
    ITERATIONS = 1
    RANDOM_NODES_LST = [500]
    percentile_90_sum = 0
    lwc_diameter = 0
    for _ in range(ITERATIONS):
        for nodes_to_select in RANDOM_NODES_LST:
            index_to_node_map = sample(
                largest_weak_component, nodes_to_select)
            index_to_node_map.sort()
            dist_list = getDistancesListParallelBFS(
                undir_adj_list, index_to_node_map, nodes_to_select)
            dist_list.sort()
            percentile_90_ind = round(nodes_to_select * 0.9)
            lwc_diameter = max(lwc_diameter, dist_list[-1])
            percentile_90_sum += dist_list[percentile_90_ind]
    print("LWC diameter with distance with random vertices: "
          f"{lwc_diameter}")
    print("LWC 90th percentile distance with random verices: "
          f"{round(percentile_90_sum / ITERATIONS)}")


def displayLWCDiamAndPercentWithSnowball(undir_adj_list: Tuple[Set[int]],
                                         largest_weak_component: List[int]):
    """
    Using selected random nodes constructs snowball graph and prints:
        1. Estimated largest weak component diameter
        2. Estimated 90th percentile distance between vertices of LWC
    """
    # TODO still needs to be implemented
    ITERATIONS = 1
    INIT_ADJ_NODES_NUMBER = 3
    SNOWBALL_GRAPH_LIMIT_SIZE = 1000
    for _ in range(ITERATIONS):
        init_adj_nodes_list = []
        for node in largest_weak_component:
            if len(undir_adj_list[node]) >= INIT_ADJ_NODES_NUMBER - 1:
                init_adj_nodes_list.append(node)
                to_append = INIT_ADJ_NODES_NUMBER - 1
                for adj_node in undir_adj_list[node]:
                    init_adj_nodes_list.append(adj_node)
                    to_append -= 1
                    if to_append == 0:
                        break
                break
        snowball_subgraph = createInducedSubgraph(undir_adj_list,
                                                  getNodeListWithSnowballBFS(
                                                      undir_adj_list,
                                                      init_adj_nodes_list,
                                                      SNOWBALL_GRAPH_LIMIT_SIZE
                                                  ))


def processFile(file_path: str) -> None:
    """
    Process given file using all three methods
    """
    print(f"Processing: {file_path}")
    undir_adj_list = createUndirAdjList(file_path)
    largest_weak_component = getLargestWeakComponent(undir_adj_list)
    print(f"LWC size: {len(largest_weak_component)}")
    displayLWCDiamWithTwoBFS(undir_adj_list, largest_weak_component)
    displayLWCDiamAndPercentWithRandVertDist1(
        undir_adj_list, largest_weak_component)


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)
