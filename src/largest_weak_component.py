from typing import Tuple, Set, List

from network_structure_analysis.A.two.bfs import getWeakComponentBFS


def getLargestWeakComponent(undir_adj_list: Tuple[Set[int], ...]) -> List[int]:
    """
    Returns list of nodes corresponding to LWC
    """
    visited = list(False for _ in range(len(undir_adj_list)))
    largest_weak_component = []
    for i in range(len(undir_adj_list)):
        new_weak_component = []
        if not visited[i]:
            new_weak_component = getWeakComponentBFS(undir_adj_list, visited, i)
        if len(largest_weak_component) < len(new_weak_component):
            largest_weak_component = new_weak_component.copy()
    return largest_weak_component
