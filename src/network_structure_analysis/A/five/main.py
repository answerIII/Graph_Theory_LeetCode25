from typing import Tuple, Set

from os import makedirs

from pathlib import Path

from create_adj_list import createUndirAdjList

from definitions import (
    INF,
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
    IMAGES_VERTEX_DEGREE_PROB_COMMON_SCALE,
    IMAGES_VERTEX_DEGREE_PROB_LOGLOG_SCALE,
)

from save_func_pictures import saveNodeDegreeProbFunc


def getNodeDegreesInfo(
    undir_adj_list: Tuple[Set[int], ...],
) -> Tuple[int, int, float]:
    """
    Returns tuple containing:
        1. max_degree
        2. min_degree
        3. avg_degree
    """
    max_degree = -INF
    min_degree = INF
    degree_sum = 0
    total_nodes = len(undir_adj_list)
    for node_ind in range(total_nodes):
        new_degree = len(undir_adj_list[node_ind])
        max_degree = max(max_degree, new_degree)
        min_degree = min(min_degree, new_degree)
        degree_sum += new_degree
    avg_degree = degree_sum / total_nodes
    return (min_degree, max_degree, avg_degree)


def processFile(file_path: str) -> None:
    """
    Process given file using all three methods
    """
    print(f"Processing: {file_path}")
    undir_adj_list = createUndirAdjList(file_path)
    degree_info: Tuple[int, int, float] = getNodeDegreesInfo(undir_adj_list)
    print(f"Min undir graph degree: {degree_info[0]}")
    print(f"Max undir graph degree: {degree_info[1]}")
    print(f"Average undir graph degree: {degree_info[2]}")
    graph_name = file_path[file_path.rfind("/") + 1 :]
    makedirs(Path(IMAGES_VERTEX_DEGREE_PROB_COMMON_SCALE), exist_ok=True)
    makedirs(Path(IMAGES_VERTEX_DEGREE_PROB_LOGLOG_SCALE), exist_ok=True)
    saveNodeDegreeProbFunc(undir_adj_list, graph_name)


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)
