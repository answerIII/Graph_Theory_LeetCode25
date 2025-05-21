from typing import (
    Tuple,
    Set
)

from pathlib import Path


def createUndirAdjList(input_file_path: str) -> Tuple[Set[int], ...]:
    separator = None
    if (input_file_path[-4:] == ".csv"):
        separator = ","
    with open(Path(input_file_path), mode='r') as input_file:
        nodes, _ = map(int, input_file.readline().split(sep=separator))
        adj_list: Tuple[Set[int], ...] = tuple(set() for _ in range(nodes))
        for input_file_line in input_file:
            out_node, in_node = map(int, input_file_line.split(sep=separator))
            adj_list[out_node].add(in_node)
            adj_list[in_node].add(out_node)
    return adj_list


def createDirAdjList(input_file_path: str) -> Tuple[Set[int], ...]:
    separator = None
    if (input_file_path[-4:] == ".csv"):
        separator = ","
    with open(Path(input_file_path), mode='r') as input_file:
        nodes, _ = map(int, input_file.readline().split(sep=separator))
        adj_list: Tuple[Set[int], ...] = tuple(set() for _ in range(nodes))
        for input_file_line in input_file:
            out_node, in_node = map(int, input_file_line.split(sep=separator))
            adj_list[out_node].add(in_node)
    return adj_list
