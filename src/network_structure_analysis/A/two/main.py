from random import choice, sample

from typing import List, Tuple, Set, Dict

from create_adj_list import createUndirAdjList

from definitions import (
    INF,
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
)

from network_structure_analysis.A.two.bfs import (
    getWeakComponentBFS,
    getFurthestNodeBFS,
    updateDistNodeSubsetBFS,
    updateDistBFS,
    getDistancesListParallelBFS,
    getNodeListWithSnowballBFS,
)

from network_structure_analysis.A.two.create_induced_subgraph import (
    createInducedSubgraph,
)


def getLargestWeakComponent(undir_adj_list: Tuple[Set[int], ...]) -> List[int]:
    visited = list(False for _ in range(len(undir_adj_list)))
    largest_weak_component = []
    for i in range(len(undir_adj_list)):
        new_weak_component = []
        if not visited[i]:
            new_weak_component = getWeakComponentBFS(undir_adj_list, visited, i)
        if len(largest_weak_component) < len(new_weak_component):
            largest_weak_component = new_weak_component.copy()
    return largest_weak_component


def getDiamAndPercentile(
    distance_triangle: List[List[int]], total_nodes: int
) -> Tuple[int, int]:
    """
    Using distance_triangle returns graph diameter and 90 percentile
    """
    sorted_dist: List[int] = []
    for i in range(total_nodes):
        for j in range(1, total_nodes - i):
            sorted_dist.append(distance_triangle[i][j])
    sorted_dist.sort()
    percentile_90_ind = round(total_nodes * 0.9)
    return (sorted_dist[-1], sorted_dist[percentile_90_ind])


def displayLWCDiamWithTwoBFS(
    undir_adj_list: Tuple[Set[int], ...], largest_weak_component: List[int]
) -> None:
    """
    Prints an estimated largest weak component diameter using a double BFS
    """
    ITERATIONS = 5
    lwc_diameter = 0
    for _ in range(ITERATIONS):
        random_node = choice(largest_weak_component)
        furthest_node, _ = getFurthestNodeBFS(undir_adj_list, random_node)
        lwc_diameter = max(
            lwc_diameter, getFurthestNodeBFS(undir_adj_list, furthest_node)[1]
        )
    print(f"LWC diameter with two BFS: {lwc_diameter}")


def displayLWCDiamAndPercentWithRandVertDist1(
    undir_adj_list: Tuple[Set[int], ...], largest_weak_component: List[int]
) -> None:
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
            selected_nodes = sample(largest_weak_component, nodes_to_select)
            selected_nodes.sort()
            node_to_ind_map: Dict[int, int] = {}
            for ind, node in enumerate(selected_nodes):
                node_to_ind_map[node] = ind
            distance_triangle = [
                [INF for _ in range(nodes_to_select - i)]
                for i in range(nodes_to_select)
            ]
            nodes_to_update = nodes_to_select
            for start_node_ind in selected_nodes:
                updateDistNodeSubsetBFS(
                    undir_adj_list,
                    distance_triangle,
                    node_to_ind_map,
                    start_node_ind,
                    nodes_to_update,
                )
                nodes_to_update -= 1
            new_lwc_diameter, new_percentile_90 = getDiamAndPercentile(
                distance_triangle, nodes_to_select
            )
            lwc_diameter = max(lwc_diameter, new_lwc_diameter)
            percentile_90_sum += new_percentile_90
    print(f"LWC diameter with distance between random vertices: {lwc_diameter}")
    print(
        "LWC 90th percentile with distance between random vertices: "
        f"{round(percentile_90_sum / ITERATIONS)}"
    )


def displayLWCDiamAndPercentWithRandVertDist2(
    undir_adj_list: Tuple[Set[int], ...], largest_weak_component: List[int]
) -> None:
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
            index_to_node_map = sample(largest_weak_component, nodes_to_select)
            index_to_node_map.sort()
            dist_list = getDistancesListParallelBFS(
                undir_adj_list, index_to_node_map, nodes_to_select
            )
            dist_list.sort()
            percentile_90_ind = round(nodes_to_select * 0.9)
            lwc_diameter = max(lwc_diameter, dist_list[-1])
            percentile_90_sum += dist_list[percentile_90_ind]
    print(f"LWC diameter with distance between random vertices: {lwc_diameter}")
    print(
        "LWC 90th percentile with distance between random vertices: "
        f"{round(percentile_90_sum / ITERATIONS)}"
    )


def displayLWCDiamAndPercentWithSnowball(
    undir_adj_list: Tuple[Set[int], ...], largest_weak_component: List[int]
):
    """
    Using selected random nodes constructs snowball graph and prints:
        1. Estimated largest weak component diameter
        2. Estimated 90th percentile distance between vertices of LWC
    """
    ITERATIONS = 5
    INIT_ADJ_NODES_NUMBER = 3
    SNOWBALL_GRAPH_LIMIT_SIZE = 1000
    percentile_90_sum = 0
    lwc_diameter = 0
    for _ in range(ITERATIONS):
        init_adj_nodes_list: List[int] = []
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
        snowball_subgraph = createInducedSubgraph(
            undir_adj_list,
            getNodeListWithSnowballBFS(
                undir_adj_list, init_adj_nodes_list, SNOWBALL_GRAPH_LIMIT_SIZE
            ),
        )
        snowball_subgraph_total_nodes = len(snowball_subgraph)
        distance_triangle = [
            [INF for _ in range(snowball_subgraph_total_nodes - i)]
            for i in range(snowball_subgraph_total_nodes)
        ]
        nodes_to_update = snowball_subgraph_total_nodes
        for start_node_ind in range(snowball_subgraph_total_nodes):
            updateDistBFS(
                snowball_subgraph,
                distance_triangle,
                start_node_ind,
                nodes_to_update,
            )
            nodes_to_update -= 1
        new_lwc_diameter, new_percentile_90 = getDiamAndPercentile(
            distance_triangle, snowball_subgraph_total_nodes
        )
        lwc_diameter = max(lwc_diameter, new_lwc_diameter)
        percentile_90_sum += new_percentile_90
    print(f"LWC diameter with snowball subgraph: {lwc_diameter}")
    print(
        "LWC 90th percentile with snowball subgraph: "
        f"{round(percentile_90_sum / ITERATIONS)}"
    )


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
        undir_adj_list, largest_weak_component
    )
    displayLWCDiamAndPercentWithSnowball(undir_adj_list, largest_weak_component)


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)
