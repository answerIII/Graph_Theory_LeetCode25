from typing import Tuple, Set, List

from bfs import getWeakComponentBFS


def getLargestWeakComponent(
    undir_adj_list: Tuple[Set[int], ...],
) -> List[int]:
    """
    Returns list of nodes corresponding to LWC
    """
    visited = list(False for _ in range(len(undir_adj_list)))
    largest_weak_component_lst = []
    for i in range(len(undir_adj_list)):
        new_weak_component = []
        if not visited[i]:
            new_weak_component = getWeakComponentBFS(undir_adj_list, visited, i)
        if len(largest_weak_component_lst) < len(new_weak_component):
            largest_weak_component_lst = new_weak_component.copy()
    return largest_weak_component_lst
