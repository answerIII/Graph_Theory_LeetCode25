from analyze_graph import analyze_network

from definitions import (
    DIRECTED_FILE_NAMES,
    UNDIRECTED_FILE_NAMES,
    LARGE_UNDIRECTED_FILE_NAMES,
    REF_DATASETS_DIRECTED_DIR,
    REF_DATASETS_UNDIRECTED_DIR,
    REF_DATASETS_LARGE_DIR,
)


def processFile(file_path: str, directed: bool) -> None:
    print(f"Processing: {file_path}")
    result = analyze_network(file_path, directed)
    print(f"Vertices: {result['vertices']}")
    print(f"Edges: {result['edges']}")
    print(f"Density: {result['density']}")
    print(f"Weak components: {result['weak_components']}")
    print(f"Max weak component ratio: {result['max_weak_ratio']}")
    if result["strong_components"] is not None:
        print(f"Strong components: {result['strong_components']}")
        print(f"Max strong component ratio: {result['max_strong_ratio']}")
    print("---")


if __name__ == "__main__":
    for directed_file_name in DIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_DIRECTED_DIR + directed_file_name, directed=True)
    for undirected_file_name in UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_UNDIRECTED_DIR + undirected_file_name, directed=False)
    for large_undirected_file_name in LARGE_UNDIRECTED_FILE_NAMES:
        processFile(REF_DATASETS_LARGE_DIR + large_undirected_file_name, directed=False)
