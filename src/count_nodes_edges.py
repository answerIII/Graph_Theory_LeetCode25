from typing import Tuple

from pathlib import Path


def countNodesEdges(input_file_path: str) -> Tuple[int, int]:
    with open(Path(input_file_path), mode="r") as input_file:
        max_node = 0
        edges = 0
        for input_file_line in input_file:
            edges += 1
            out_node, in_node = map(int, input_file_line.split())
            max_node = max(max_node, out_node)
            max_node = max(max_node, in_node)
    return (max_node + 1, edges)
