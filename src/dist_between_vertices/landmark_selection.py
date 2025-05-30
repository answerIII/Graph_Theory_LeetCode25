from collections import deque

from random import sample

from typing import List, Tuple, Set, Deque, Dict

from functools import cmp_to_key

from dist_between_vertices.compare import makeNodeDegreeCompare

from bfs import getShortestPathNodesBFS


def chooseRandomNodes(
    undir_adj_list: Tuple[Set[int], ...], nodes_to_choose: int
) -> List[int]:
    return sample(range(len(undir_adj_list)), nodes_to_choose)


def chooseHighestDegreeNodes(
    undir_adj_list: Tuple[Set[int], ...], nodes_to_choose: int
) -> List[int]:
    total_nodes = len(undir_adj_list)
    sorted_degree_nodes = sorted(
        list(i for i in range(total_nodes)),
        key=cmp_to_key(makeNodeDegreeCompare(undir_adj_list)),
        reverse=True,
    )
    return sorted_degree_nodes[0:nodes_to_choose]


def chooseBestCoverageNodes(
    undir_adj_list: Tuple[Set[int], ...], nodes_to_choose: int
) -> List[int]:
    total_nodes = len(undir_adj_list)
    PATHS_TO_CALCULATE = nodes_to_choose * 40
    calculated_paths: Set[Tuple[int, int]] = set()
    paths_lst: Deque[Set[int]] = deque()
    nodes_freq: Dict[int, int] = {}
    for _ in range(PATHS_TO_CALCULATE):
        node1_ind, node2_ind = sample(range(total_nodes), 2)
        while (node1_ind, node2_ind) in calculated_paths:
            node1_ind, node2_ind = sample(range(total_nodes), 2)
        paths_lst.append(
            getShortestPathNodesBFS(undir_adj_list, node1_ind, node2_ind)
        )
        for node_ind in paths_lst[-1]:
            if node_ind not in nodes_freq:
                nodes_freq[node_ind] = 0
            nodes_freq[node_ind] += 1
        calculated_paths.add((node1_ind, node2_ind))
    del calculated_paths
    landmark_lst: List[int] = []
    for _ in range(nodes_to_choose):
        max_freq = 0
        max_freq_node = -1
        for node, freq in nodes_freq.items():
            if freq > max_freq:
                max_freq = freq
                max_freq_node = node
        for path in paths_lst:
            if max_freq_node in path:
                for node in path:
                    nodes_freq[node] -= 1
                path.clear()
        nodes_freq.pop(max_freq_node)
        landmark_lst.append(max_freq_node)
    return landmark_lst
