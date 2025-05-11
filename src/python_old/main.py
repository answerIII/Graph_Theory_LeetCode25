import sys
import os
from graph import Graph

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 main.py <graph_file> <output_file>")
        sys.exit(1)

    file_path = sys.argv[1]
    output_path = sys.argv[2]

    graph_name = os.path.splitext(os.path.basename(file_path))[0]

    graph: 'Graph' = Graph.from_file(file_path, directed=True)
    ugraph: 'Graph' = graph.cast_dgraph_to_ugraph(graph, directed=False)

    wcc = Graph.sort_components(ugraph.find_wcc(), desc=True)
    scc = Graph.sort_components(graph.find_scc(), desc=True)

    with open(output_path, 'w') as f:
        f.write(f"Сводная информация о графе {graph_name}:\n\n")
        f.write(f"Количество вершин: {ugraph.number_of_nodes()}\n")
        f.write(f"Количество рёбер: {ugraph.number_of_edges()}\n")
        f.write(f"Плотность графа: {ugraph.density():.6f}\n")
        f.write(f"Количество WCC: {len(wcc)}\n")
        f.write(f"Доля вершин в максимальной WCC: {len(wcc[0]) / graph.number_of_nodes():.6f}\n")
        f.write(f"Количество SCC: {len(scc)}\n")
        f.write(f"Доля вершин в максимальной SCC: {len(scc[0]) / graph.number_of_nodes():.6f}\n")
