from typing import Tuple, Set


def makeNodeDegreeCompare(undir_adj_list: Tuple[Set[int], ...]):
    def compareNodeDegree(left_node: int, right_node: int):
        left_node_degree = len(undir_adj_list[left_node])
        right_node_degree = len(undir_adj_list[right_node])
        if left_node_degree < right_node_degree:
            return -1
        elif left_node_degree > right_node_degree:
            return 1
        else:
            return 0

    return compareNodeDegree
