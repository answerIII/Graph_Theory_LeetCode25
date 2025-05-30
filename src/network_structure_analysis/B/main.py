from typing import Tuple, Set

from random import sample

from create_adj_list import createUndirAdjList

from largest_weak_component import getLargestWeakComponent

from definitions import (
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
)

def createNewList(
        undir_adj_list: Tuple[Set[int], ...],
        new_nodes: set[int],
) -> Tuple[Set[int], ...]:
    """
    Returns adj list of new random subgraph
    """
    new_list = tuple(set() for _ in range(len(undir_adj_list)))
    for v in new_nodes:
        for u in undir_adj_list[v]:
            if u in new_nodes:
                new_list[v].add(u)
    return new_list


def deleteRandomX(
        undir_adj_list: Tuple[Set[int], ...],
        x: int,
) -> (int, Tuple[Set[int], ...]):
    """ """
    new_size = round(len(undir_adj_list) * (100 - x) / 100)
    return (new_size, createNewList(
        undir_adj_list, set(sample(range(len(undir_adj_list)), new_size))
    ))


def deleteRandomLargestDegreeX(
        undir_adj_list: Tuple[Set[int], ...],
        x: int,
) -> (int, Tuple[Set[int], ...]):
    """ """
    new_size = round(len(undir_adj_list) * (100 - x) / 100)
    return (new_size,
            createNewList(
        undir_adj_list,
        set(
            [
                i
                for _, i in sorted(
                ((len(s), i) for i, s in enumerate(undir_adj_list))
            )
            ][:new_size]
        ),
    ))

def processFile(file_path: str) -> None:
    print(f"Processing: {file_path}")
    undir_adj_list = createUndirAdjList(file_path)
    number_of_generations = 1
    original_ratio = len(getLargestWeakComponent(undir_adj_list)) / len(
        undir_adj_list
    )
    print(f"Original ratio: {original_ratio}")
    for x in range(1, 100, 5):
        mean_ratio = 0
        for _ in range(number_of_generations):
            size, subgraph = deleteRandomX(undir_adj_list, x)
            mean_ratio += len(getLargestWeakComponent(subgraph)) / size
        mean_ratio /= number_of_generations
        print(f"After deleting {x}% (mean value): {mean_ratio}")
    for x in range(1, 100, 5):
        size, subgraph = deleteRandomLargestDegreeX(undir_adj_list, x)
        print(
            f"After deleting {x}% of largest by degree: {len(getLargestWeakComponent(subgraph)) / size}"
        )

if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name)
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name)