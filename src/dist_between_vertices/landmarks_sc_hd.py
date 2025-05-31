from random import sample

from time import time

from definitions import (
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
)

from dist_between_vertices.landmark_selection import chooseHighestDegreeNodes

from bfs import getDistNodesPair

from create_adj_list import createLWCUndirAdjList

from dist_between_vertices.create_spt import createSPTList

from dist_between_vertices.landmarks_sc import landmarksSC


def processFile(file_path: str) -> None:
    print(f"Processing: {file_path}")
    lwc_undir_adj_list = createLWCUndirAdjList(file_path)
    lwc_nodes = len(lwc_undir_adj_list)
    total_landmarks_lst = [5, 10, 20]
    for landmarks_num in total_landmarks_lst:
        total_start_time = time()
        choose_hd_nodes_start_time = time()
        landmarks_lst = chooseHighestDegreeNodes(
            lwc_undir_adj_list, landmarks_num
        )
        choose_hd_nodes_time = time() - choose_hd_nodes_start_time
        create_spt_lst_start_time = time()
        spt_lst = createSPTList(lwc_undir_adj_list, landmarks_lst)
        create_spt_lst_time = time() - create_spt_lst_start_time
        TOTAL_DIST_COMPUTES = 100
        hd_dist_sum = 0
        real_dist_sum = 0
        hd_lm_sc_time = 0
        for _ in range(TOTAL_DIST_COMPUTES):
            node1_ind, node2_ind = sample(range(lwc_nodes), 2)
            hd_lm_sc_start_time = time()
            hd_dist_sum += landmarksSC(
                lwc_undir_adj_list,
                spt_lst,
                landmarks_lst,
                landmarks_num,
                node1_ind,
                node2_ind,
            )
            hd_lm_sc_time += time() - hd_lm_sc_start_time
            real_dist_sum += getDistNodesPair(
                lwc_undir_adj_list, node1_ind, node2_ind
            )
        print(f"Landmarks amount: {landmarks_num}")
        print("\tTime")
        print(f"\t\tChoose highest degree nodes: {choose_hd_nodes_time} sec")
        print(f"\t\tPrecompute SPT list: {create_spt_lst_time} sec")
        print(
            f"\t\tCalculation of {TOTAL_DIST_COMPUTES} distances for "
            f"highest degree nodes: {hd_lm_sc_time} sec"
        )
        print(f"\t\tTotal: {time() - total_start_time} sec")
        print("\tAccuracy")
        print(
            "\t\tLandmarks-SC highest degree landmarks: "
            f"{real_dist_sum / hd_dist_sum}"
        )


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
