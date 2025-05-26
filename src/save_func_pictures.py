from typing import Tuple, Set, Dict, List

import matplotlib.pyplot as plt
from numpy import linspace

from definitions import (
    IMAGES_VERTEX_DEGREE_PROB_COMMON_SCALE,
    IMAGES_VERTEX_DEGREE_PROB_LOGLOG_SCALE,
)


def saveNodeDegreeProbFunc(
    undir_adj_list: Tuple[Set[int], ...], graph_name: str
) -> None:
    """
    Saves graphs of probability functions for vertex degrees in common and
    log log scales
    """
    image_name = graph_name[: graph_name.find(".")] + ".png"
    total_nodes = len(undir_adj_list)
    degree_freq: Dict[int, int] = {}
    for node_ind in range(total_nodes):
        new_node_degree = len(undir_adj_list[node_ind])
        if new_node_degree not in degree_freq:
            degree_freq[new_node_degree] = 0
        degree_freq[new_node_degree] += 1
    degree_freq_size = len(degree_freq)
    degree_prob_pair_lst: List[Tuple[int, float]] = []
    for degree, freq in degree_freq.items():
        degree_prob_pair_lst.append((degree, freq / total_nodes))
    degree_prob_pair_lst.sort()
    nodes_to_plot = int(degree_freq_size * 0.1)
    plotted_pairs_ind = linspace(
        0, degree_freq_size - 1, nodes_to_plot, dtype=int
    )
    label_x_data = list(degree_prob_pair_lst[i][0] for i in plotted_pairs_ind)
    label_y_data = list(degree_prob_pair_lst[i][1] for i in plotted_pairs_ind)
    plt.plot(label_x_data, label_y_data)
    plt.title(
        "Common scale\n"
        f"Probability function for vertex degrees for graph {graph_name}"
    )
    plt.xlabel("Vertex degree")
    plt.ylabel("Probability")
    plt.savefig(IMAGES_VERTEX_DEGREE_PROB_COMMON_SCALE + image_name)
    plt.close()
    plt.loglog(label_x_data, label_y_data)
    plt.title(
        "Loglog scale\n"
        f"Probability function for vertex degrees for graph {graph_name}"
    )
    plt.xlabel("Vertex degree")
    plt.ylabel("Probability")
    plt.savefig(IMAGES_VERTEX_DEGREE_PROB_LOGLOG_SCALE + image_name)
    plt.close()
