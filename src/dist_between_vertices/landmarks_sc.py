from typing import List, Dict, Tuple, Set

from random import sample

from definitions import (
    INF,
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
)

from create_adj_list import createLWCUndirAdjList

from dist_between_vertices.create_spt import createSPTList


def getDistSC(
    undir_adj_list: Tuple[Set[int], ...],
    shortest_path_tree: Dict[int, int],
    tree_root: int,
    node1_ind: int,
    node2_ind: int,
) -> int:
    node1_dist = 0
    node2_dist = 0
    curr_node1_ind = node1_ind
    curr_node2_ind = node2_ind
    node1_dist_map = {node1_ind: 0}
    node2_dist_map = {node2_ind: 0}
    while True:
        if curr_node1_ind != tree_root:
            for neighbour in undir_adj_list[curr_node1_ind]:
                if neighbour not in node1_dist_map:
                    if neighbour in node2_dist_map:
                        return node2_dist_map[neighbour] + node1_dist + 1
                    node1_dist_map[neighbour] = node1_dist + 1
            node1_dist += 1
            curr_node1_ind = shortest_path_tree[curr_node1_ind]
        if curr_node2_ind != tree_root:
            for neighbour in undir_adj_list[curr_node2_ind]:
                if neighbour not in node2_dist_map:
                    if neighbour in node1_dist_map:
                        return node1_dist_map[neighbour] + node2_dist + 1
                    node2_dist_map[neighbour] = node2_dist + 1
            node2_dist += 1
            curr_node2_ind = shortest_path_tree[curr_node2_ind]


def landmarksSC(
    undir_adj_list: Tuple[Set[int], ...],
    spt_lst: List[Dict[int, int]],
    landmarks_lst: List[int],
    total_landmarks: int,
    node1_ind: int,
    node2_ind: int,
) -> int:
    """
    Returns the estimated distance between nodes using shortcutting
    """
    if node1_ind == node2_ind:
        return 0
    est_dist = INF
    for i in range(total_landmarks):
        est_dist = min(
            est_dist,
            getDistSC(
                undir_adj_list,
                spt_lst[i],
                landmarks_lst[i],
                node1_ind,
                node2_ind,
            ),
        )
    return est_dist


def processFile(file_path: str) -> None:
    print(f"Processing: {file_path}")
    TOTAL_LANDMARKS = 20
    lwc_undir_adj_list = createLWCUndirAdjList(file_path)
    lwc_nodes = len(lwc_undir_adj_list)
    # Landmarks chosen randomly
    # TODO add new approaches for landmarks selection
    landmarks_lst = sample(range(lwc_nodes), TOTAL_LANDMARKS)
    spt_lst = createSPTList(lwc_undir_adj_list, landmarks_lst)
    # TODO The code below is only for demonstration of the algorithm's work
    # Delete it later
    TOTAL_DIST_COMPUTES = 1000
    dist_sum = 0
    for _ in range(TOTAL_DIST_COMPUTES):
        node1_ind, node2_ind = sample(range(lwc_nodes), 2)
        dist_sum += landmarksSC(
            lwc_undir_adj_list,
            spt_lst,
            landmarks_lst,
            TOTAL_LANDMARKS,
            node1_ind,
            node2_ind,
        )
    print(
        f"Avg estimated dist with Landmark-SC: {dist_sum / TOTAL_DIST_COMPUTES}"
    )


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    # for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
    #     processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)
