from typing import List, Tuple, Set, Dict
import time

from create_adj_list import createUndirAdjList

from definitions import (
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
)


def localClustering(
        undir_adj_list: Tuple[Set[int], ...], u: int
) -> (int, float):
    neighbors = undir_adj_list[u]
    k_u = len(neighbors)
    if k_u < 2:
        return 0, 0.0
    triangles_u = 0
    for v in neighbors:
        for w in neighbors:
            if w > v and w in undir_adj_list[v]:
                triangles_u += 1
    max_possible = k_u * (k_u - 1) / 2
    return triangles_u, max_possible


def averageClustering(undir_adj_list: Tuple[Set[int], ...]) -> (int, float):
    total = 0.0
    triangles = 0
    n = len(undir_adj_list)
    for u in range(n):
        triangles_u, max_possible = localClustering(undir_adj_list, u)
        total += triangles_u / max_possible if max_possible > 0 else 0
        triangles += triangles_u
    return triangles, total / n


def globalClustering(
        undir_adj_list: Tuple[Set[int], ...], sum_of_triangles: int
) -> float:
    triples = 0
    for u in range(len(undir_adj_list)):
        k_u = len(undir_adj_list[u])
        triples += k_u * (k_u - 1) // 2
    return sum_of_triangles / triples if triples > 0 else 0.0


def processFile(file_path: str) -> None:
    print(f"Processing: {file_path}")
    undir_adj_list = createUndirAdjList(file_path)
    sum_of_triangles, average_clustering_coef = averageClustering(
        undir_adj_list
    )
    global_clustering_coef = globalClustering(undir_adj_list, sum_of_triangles)
    triangles = sum_of_triangles // 3
    print(f"Number of triangles: {triangles}")
    print(f"Average clustering coefficient: {average_clustering_coef}")
    print(f"Global clustering coefficient: {global_clustering_coef}")


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    end = time.time()
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)