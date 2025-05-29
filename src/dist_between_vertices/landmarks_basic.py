from typing import List, Tuple, Set

from random import sample

from create_adj_list import createLWCUndirAdjList

from definitions import (
    INF,
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
)

from bfs import updateLandmarkDistBFS


def precomputeLandmarksDist(
    undir_adj_list: Tuple[Set[int], ...],
    landmarks_dist_matrix: Tuple[List[int], ...],
    landmarks_lst: List[int],
) -> None:
    """
    Calculate distances from all landmarks to all other nodes
    Store distances into landmarks_dist_matrix
    """
    for i in range(len(landmarks_lst)):
        updateLandmarkDistBFS(
            undir_adj_list, landmarks_dist_matrix, landmarks_lst[i], i
        )


def landmarksBasic(
    landmarks_dist_matrix: Tuple[List[int], ...],
    total_landmarks: int,
    node1_ind: int,
    node2_ind: int,
) -> int:
    """
    Returns the estimated distance between nodes using basic approach
    """
    if node1_ind == node2_ind:
        return 0
    est_dist = INF
    for i in range(total_landmarks):
        est_dist = min(
            est_dist,
            landmarks_dist_matrix[i][node1_ind]
            + landmarks_dist_matrix[i][node2_ind],
        )
    return est_dist


def processFile(file_path: str) -> None:
    print(f"Processing: {file_path}")
    TOTAL_LANDMARKS = 20
    lwc_undir_adj_list = createLWCUndirAdjList(file_path)
    lwc_nodes = len(lwc_undir_adj_list)
    landmarks_dist_matrix: Tuple[List[int], ...] = tuple(
        list(INF for _ in range(lwc_nodes)) for _ in range(TOTAL_LANDMARKS)
    )
    # Landmarks chosen randomly
    # TODO add new approaches for landmarks selection
    landmarks_lst = sample(range(lwc_nodes), TOTAL_LANDMARKS)
    precomputeLandmarksDist(
        lwc_undir_adj_list, landmarks_dist_matrix, landmarks_lst
    )
    # TODO The code below is only for demonstration of the algorithm's work
    # Delete it later
    TOTAL_DIST_COMPUTES = 1000
    dist_sum = 0
    for _ in range(TOTAL_DIST_COMPUTES):
        node1_ind, node2_ind = sample(range(lwc_nodes), 2)
        dist_sum += landmarksBasic(
            landmarks_dist_matrix, TOTAL_LANDMARKS, node1_ind, node2_ind
        )
    print(
        "Avg estimated dist with Landmark-Basic: "
        f"{dist_sum / TOTAL_DIST_COMPUTES}"
    )


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    # for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
    #     processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)
