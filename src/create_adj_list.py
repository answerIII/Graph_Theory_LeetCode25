from typing import Tuple, Set, Dict

from pathlib import Path

from largest_weak_component import getLargestWeakComponent


def createUndirAdjList(input_file_path: str) -> Tuple[Set[int], ...]:
    separator = None
    if input_file_path[-4:] == ".csv":
        separator = ","
    with open(Path(input_file_path), mode="r") as input_file:
        nodes, _ = map(int, input_file.readline().split(sep=separator))
        adj_list: Tuple[Set[int], ...] = tuple(set() for _ in range(nodes))
        for input_file_line in input_file:
            out_node, in_node = map(int, input_file_line.split(sep=separator))
            adj_list[out_node].add(in_node)
            adj_list[in_node].add(out_node)
    return adj_list


def createDirAdjList(input_file_path: str) -> Tuple[Set[int], ...]:
    separator = None
    if input_file_path[-4:] == ".csv":
        separator = ","
    with open(Path(input_file_path), mode="r") as input_file:
        nodes, _ = map(int, input_file.readline().split(sep=separator))
        adj_list: Tuple[Set[int], ...] = tuple(set() for _ in range(nodes))
        for input_file_line in input_file:
            out_node, in_node = map(int, input_file_line.split(sep=separator))
            adj_list[out_node].add(in_node)
    return adj_list


def createUndirAdjListFromWeakComp(
    input_file_path: str,
    largest_weak_component_node_map: Dict[int, int],
) -> Tuple[Set[int], ...]:
    """
    Returns weak component graph where node_id for each node in segment
    [0, len(wek_component) - 1]
    """
    separator = None
    if input_file_path[-4:] == ".csv":
        separator = ","
    with open(Path(input_file_path), mode="r") as input_file:
        input_file.readline()
        wc_undir_adj_list: Tuple[Set[int], ...] = tuple(
            set() for _ in range(len(largest_weak_component_node_map))
        )
        for input_file_line in input_file:
            out_node, in_node = map(int, input_file_line.split(sep=separator))
            if out_node in largest_weak_component_node_map:
                new_out_node = largest_weak_component_node_map[out_node]
                new_in_node = largest_weak_component_node_map[in_node]
                wc_undir_adj_list[new_out_node].add(new_in_node)
                wc_undir_adj_list[new_in_node].add(new_out_node)
    return wc_undir_adj_list


def createLWCUndirAdjList(input_file_path: str) -> Tuple[Set[int], ...]:
    """
    Find LWC of graph given as input_file_path
    Returns LWC graph where node_id for each node in segment [0, len(lwc) - 1]
    """
    undir_adj_list = createUndirAdjList(input_file_path)
    largest_weak_component_lst = getLargestWeakComponent(undir_adj_list)
    if len(largest_weak_component_lst) == len(undir_adj_list):
        return undir_adj_list
    else:
        del undir_adj_list
        largest_weak_component_node_map: Dict[int, int] = {}
        largest_weak_component_lst.sort()
        for new_node_ind, old_node_ind in enumerate(largest_weak_component_lst):
            largest_weak_component_node_map[old_node_ind] = new_node_ind
        del largest_weak_component_lst
        return createUndirAdjListFromWeakComp(
            input_file_path, largest_weak_component_node_map
        )
