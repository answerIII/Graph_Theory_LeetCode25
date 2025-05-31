from typing import List, Dict, Tuple, Set

from definitions import INF


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
