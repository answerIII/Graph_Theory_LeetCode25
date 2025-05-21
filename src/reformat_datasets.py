from typing import (
    Dict,
    TextIO,
    Optional,
    Set,
    List
)

from definitions import (
    DATASETS_DIRECTED_DIR,
    DATASETS_UNDIRECTED_DIR,
    DATASETS_LARGE_DIR,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR
)
from pathlib import Path


def getNodesMapping(input_file: TextIO, separator: Optional[str] = None) \
        -> tuple[Dict[int, int], int]:
    """
    Returns mapping from old_node_id to new_node_id
    """
    unique_nodes: Set[int] = set()
    sorted_nodes: List[int] = list()
    input_file.readline()
    total_edges = 0
    for input_file_line in input_file:
        total_edges += 1
        node1, node2, *_ = map(int, input_file_line.split(sep=separator))
        if node1 not in unique_nodes:
            unique_nodes.add(node1)
            sorted_nodes.append(node1)
        if node2 not in unique_nodes:
            unique_nodes.add(node2)
            sorted_nodes.append(node2)
    sorted_nodes.sort()
    new_nodes_id_map: Dict[int, int] = dict()
    for new_id, old_id in enumerate(sorted_nodes):
        new_nodes_id_map[old_id] = new_id
    return new_nodes_id_map, total_edges


def reformatTXTGraphFile(input_file_path: str, output_file_path: str) -> None:
    input_file = open(Path(input_file_path), mode='r')
    new_nodes_id_map, _ = getNodesMapping(input_file)
    input_file.seek(0)
    output_file = open(Path(output_file_path), mode='w')
    output_file.write(input_file.readline())
    for input_file_line in input_file:
        node1, node2 = map(int, input_file_line.split())
        output_file.write(
            f"{new_nodes_id_map[node1]} {new_nodes_id_map[node2]}\n")
    output_file.close()
    input_file.close()


def reformatCSVGraphFile(input_file_path: str, output_file_path: str) -> None:
    input_file = open(Path(input_file_path), mode='r')
    new_nodes_id_map, total_edges = getNodesMapping(input_file, separator=",")
    input_file.seek(0)
    output_file = open(Path(output_file_path), mode='w')
    input_file.readline()
    output_file.write(f"{len(new_nodes_id_map)},{total_edges}\n")
    for input_file_line in input_file:
        node1, node2, *_ = map(int, input_file_line.split(sep=","))
        output_file.write(
            f"{new_nodes_id_map[node1]},{new_nodes_id_map[node2]}\n")
    output_file.close()
    input_file.close()


def reformatMTXGraphFile(input_file_path: str, output_file_path: str) -> None:
    input_file = open(Path(input_file_path), mode='r')
    new_nodes_id_map, _ = getNodesMapping(input_file)
    input_file.seek(0)
    output_file = open(Path(output_file_path), mode='w')
    sourse_nodes, dest_nodes, total_edges = map(
        int, input_file.readline().split())
    total_nodes = max(sourse_nodes, dest_nodes)
    output_file.write(f"{total_nodes} {total_edges}\n")
    for input_file_line in input_file:
        node1, node2 = map(int, input_file_line.split())
        output_file.write(
            f"{new_nodes_id_map[node1]} {new_nodes_id_map[node2]}\n")
    output_file.close()
    input_file.close()


txt_datasets_path_pairs = [
    [DATASETS_DIRECTED_DIR + "Wiki-Vote.txt",
        REF_DATASETS_DIRECTED_DIR + "Wiki-Vote.txt"],
    [DATASETS_DIRECTED_DIR + "web-Google.txt",
        REF_DATASETS_DIRECTED_DIR + "web-Google.txt"],
    [DATASETS_DIRECTED_DIR + "web-NotreDame.txt",
        REF_DATASETS_DIRECTED_DIR + "web-NotreDame.txt"],
    [DATASETS_DIRECTED_DIR + "web-Stanford.txt",
        REF_DATASETS_DIRECTED_DIR + "web-Stanford.txt"],
    [DATASETS_UNDIRECTED_DIR + "CA-AstroPh.txt",
        REF_DATASETS_UNDIRECTED_DIR + "CA-AstroPh.txt"],
    [DATASETS_UNDIRECTED_DIR + "CA-GrQc.txt",
        REF_DATASETS_UNDIRECTED_DIR + "CA-GrQc.txt"],
    [DATASETS_UNDIRECTED_DIR + "Email-EuAll.txt",
        REF_DATASETS_UNDIRECTED_DIR + "Email-EuAll.txt"],
    [DATASETS_UNDIRECTED_DIR + "ca-coauthors-dblp.txt",
        REF_DATASETS_UNDIRECTED_DIR + "ca-coauthors-dblp.txt"],
    [DATASETS_LARGE_DIR + "com-orkut.ungraph.txt",
        REF_DATASETS_LARGE_DIR + "com-orkut.ungraph.txt"],
    [DATASETS_LARGE_DIR + "com-youtube.ungraph.txt",
        REF_DATASETS_LARGE_DIR + "com-youtube.ungraph.txt"]
]

csv_datasets_path_pairs = [
    [DATASETS_UNDIRECTED_DIR + "musae_git_edges.csv",
        REF_DATASETS_UNDIRECTED_DIR + "musae_git_edges.csv"],
    [DATASETS_LARGE_DIR + "vk.csv",
        REF_DATASETS_LARGE_DIR + "vk.csv"]
]

mtx_dataset_path_pairs = [
    [DATASETS_DIRECTED_DIR + "soc-wiki-Vote.mtx",
        REF_DATASETS_DIRECTED_DIR + "soc-wiki-Vote.mtx"],
]


if __name__ == "__main__":
    for input_file_path, output_file_path in txt_datasets_path_pairs:
        print(f"Processing: {input_file_path}")
        reformatTXTGraphFile(input_file_path, output_file_path)
    for input_file_path, output_file_path in csv_datasets_path_pairs:
        print(f"Processing: {input_file_path}")
        reformatCSVGraphFile(input_file_path, output_file_path)
    for input_file_path, output_file_path in mtx_dataset_path_pairs:
        print(f"Processing: {input_file_path}")
        reformatMTXGraphFile(input_file_path, output_file_path)
