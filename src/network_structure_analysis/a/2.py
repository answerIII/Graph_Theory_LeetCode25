from queue import Queue

from typing import (
    List,
    Tuple,
    Set
)

from create_adj_list import (
    createUndirAdjList,
    # createDirAdjList
)

from definitions import (
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR
)


def BFSUndirGraph(undir_adj_list: Tuple[Set[int]],
                  visited: List[bool],
                  node_ind: int) -> List[int]:
    visited[node_ind] = True
    nodes_queue = Queue()
    nodes_queue.put(node_ind)
    new_weak_component = [node_ind]
    while not nodes_queue.empty():
        new_node_ind = nodes_queue.get()
        for neighbour_node_ind in undir_adj_list[new_node_ind]:
            if not visited[neighbour_node_ind]:
                visited[neighbour_node_ind] = True
                nodes_queue.put(neighbour_node_ind)
                new_weak_component.append(neighbour_node_ind)
    return new_weak_component


def getLargestWeakComponent(undir_adj_list: Tuple[Set[int]]) -> List[int]:
    visited = list(False for _ in range(len(undir_adj_list)))
    largest_weak_component = []
    for i in range(len(undir_adj_list)):
        new_weak_component = []
        if not visited[i]:
            new_weak_component = BFSUndirGraph(undir_adj_list, visited, i)
        if len(largest_weak_component) < len(new_weak_component):
            largest_weak_component = new_weak_component.copy()
    return largest_weak_component


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        print(f"Processing: {directed_file_name}")
        undir_adj_list = createUndirAdjList(
            REF_DATASETS_DIRECTED_DIR + directed_file_name)
        print(len(getLargestWeakComponent(undir_adj_list)))
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        print(f"Processing: {undirected_file_name}")
        undir_adj_list = createUndirAdjList(
            REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
        print(len(getLargestWeakComponent(undir_adj_list)))
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        print(f"Processing: {large_undirected_file_name}")
        undir_adj_list = createUndirAdjList(
            REF_DATASETS_LARGE_DIR + large_undirected_file_name)
        print(len(getLargestWeakComponent(undir_adj_list)))
