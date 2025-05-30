from time import time

from typing import List, Tuple, Set

from random import sample

from create_adj_list import createLWCUndirAdjList

from definitions import (
    INF,
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
)

from dist_between_vertices.landmark_selection import (
    chooseRandomNodes,
    chooseHighestDegreeNodes,
    chooseBestCoverageNodes,
)

from bfs import updateLandmarkDistBFS, getDistNodesPair


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
    lwc_undir_adj_list = createLWCUndirAdjList(file_path)
    lwc_nodes = len(lwc_undir_adj_list)
    total_landmarks_lst = [5, 10, 20]
    for landmarks_num in total_landmarks_lst:
        total_start_time = time()
        rand_lm_dist_matrix: Tuple[List[int], ...] = tuple(
            list(INF for _ in range(lwc_nodes)) for _ in range(landmarks_num)
        )
        hd_lm_dist_matrix: Tuple[List[int], ...] = tuple(
            list(INF for _ in range(lwc_nodes)) for _ in range(landmarks_num)
        )
        bc_lm_dist_matrix: Tuple[List[int], ...] = tuple(
            list(INF for _ in range(lwc_nodes)) for _ in range(landmarks_num)
        )
        choose_rand_nodes_start_time = time()
        rand_lm_lst = chooseRandomNodes(lwc_undir_adj_list, landmarks_num)
        choose_rand_nodes_time = time() - choose_rand_nodes_start_time
        choose_hd_nodes_start_time = time()
        hd_lm_lst = chooseHighestDegreeNodes(lwc_undir_adj_list, landmarks_num)
        choose_hd_nodes_time = time() - choose_hd_nodes_start_time
        choose_bc_nodes_start_time = time()
        bc_lm_lst = chooseBestCoverageNodes(lwc_undir_adj_list, landmarks_num)
        choose_bc_nodes_time = time() - choose_bc_nodes_start_time
        prec_lm_dist_start_time = time()
        precomputeLandmarksDist(
            lwc_undir_adj_list, rand_lm_dist_matrix, rand_lm_lst
        )
        precomputeLandmarksDist(
            lwc_undir_adj_list, hd_lm_dist_matrix, hd_lm_lst
        )
        precomputeLandmarksDist(
            lwc_undir_adj_list, bc_lm_dist_matrix, bc_lm_lst
        )
        prec_lm_dist_time = time() - prec_lm_dist_start_time
        TOTAL_DIST_COMPUTES = 100
        rand_dist_sum = 0
        hd_dist_sum = 0
        bc_dist_sum = 0
        real_dist_sum = 0
        rand_lm_basic_time = 0
        hd_lm_basic_time = 0
        bc_lm_basic_time = 0
        for _ in range(TOTAL_DIST_COMPUTES):
            node1_ind, node2_ind = sample(range(lwc_nodes), 2)
            rand_lm_basic_start_time = time()
            rand_dist_sum += landmarksBasic(
                rand_lm_dist_matrix, landmarks_num, node1_ind, node2_ind
            )
            rand_lm_basic_time += time() - rand_lm_basic_start_time
            hd_lm_basic_start_time = time()
            hd_dist_sum += landmarksBasic(
                hd_lm_dist_matrix, landmarks_num, node1_ind, node2_ind
            )
            hd_lm_basic_time += time() - hd_lm_basic_start_time
            bc_lm_basic_start_time = time()
            bc_dist_sum += landmarksBasic(
                bc_lm_dist_matrix, landmarks_num, node1_ind, node2_ind
            )
            bc_lm_basic_time += time() - bc_lm_basic_start_time
            real_dist_sum += getDistNodesPair(
                lwc_undir_adj_list, node1_ind, node2_ind
            )
        print(f"Landmarks amount: {landmarks_num}")
        print("\tTime")
        print(f"\t\tChoose rand nodes: {choose_rand_nodes_time} sec")
        print(f"\t\tChoose highest degree nodes: {choose_hd_nodes_time} sec")
        print(f"\t\tChoose best coverage nodes: {choose_bc_nodes_time} sec")
        print(f"\t\tPrecompute distances: {prec_lm_dist_time / 3} sec")
        print(
            f"\t\tCalculation of {TOTAL_DIST_COMPUTES} distances for "
            "rand nodes: "
            f"{rand_lm_basic_time} sec"
        )
        print(
            f"\t\tCalculation of {TOTAL_DIST_COMPUTES} distances for "
            "highest degree nodes: "
            f"{hd_lm_basic_time} sec"
        )
        print(
            f"\t\tCalculation of {TOTAL_DIST_COMPUTES} distances for "
            "best coverage nodes: "
            f"{bc_lm_basic_time} sec"
        )
        print(f"\t\tTotal: {time() - total_start_time} sec")
        print("\tAccuracy")
        print(
            "\t\tLandmarks-Basic random landmarks: "
            f"{real_dist_sum / rand_dist_sum}"
        )
        print(
            "\t\tLandmarks-Basic highest degree landmarks: "
            f"{real_dist_sum / hd_dist_sum}"
        )
        print(
            f"\t\tLandmarks-Basic best coverage: {real_dist_sum / bc_dist_sum}"
        )


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
