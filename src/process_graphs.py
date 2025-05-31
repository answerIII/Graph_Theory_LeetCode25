from typing import Tuple, Set

from create_adj_list import (
    createUndirAdjListNewSamples,
    createDirAdjList,
    createUndirAdjList,
)

from bfs import getFurthestNodeBFS, getDistNodesPair

from largest_weak_component import getLargestWeakComponent

from network_structure_analysis.A.three.main import countTriangles

from network_structure_analysis.A.four.main import getNodeClusterCoeff

from network_structure_analysis.A.one.analyze_graph import analyze_network

undir_graphs = [
    "ref_tests/graph_0.txt",
    "ref_tests/graph_1.txt",
    "ref_tests/graph_2.txt",
    "ref_tests/graph_3.txt",
    "ref_tests/graph_4.txt",
    "ref_tests/graph_5.txt",
    "ref_tests/graph_6.txt",
    "ref_tests/graph_7.txt",
    "ref_tests/graph_8.txt",
]

dir_graphs = [
    "ref_tests/digraph_1.txt",
    "ref_tests/digraph_2.txt",
    "ref_tests/digraph_3.txt",
]


def getGraphDiamAndRadius(
    undir_adj_list: Tuple[Set[int], ...],
) -> Tuple[int, int]:
    diameter = 0
    radius = 10**9
    total_nodes = len(undir_adj_list)
    for node_ind in range(total_nodes):
        _, new_distance = getFurthestNodeBFS(undir_adj_list, node_ind)
        diameter = max(diameter, new_distance)
        radius = min(radius, new_distance)
    return (diameter, radius)


def processUndirGraph(file_path: str) -> None:
    TEAM_NUMBER = 9
    print(f"Processing {file_path}")
    undir_adj_list, nodes, edges = createUndirAdjListNewSamples(file_path)
    print(f"\tNodes: {nodes}")
    print(f"\tEdges: {edges}")
    diameter, radius = getGraphDiamAndRadius(undir_adj_list)
    print(f"\tDiameter: {diameter}")
    print(f"\tRadius: {radius}")
    max_edges = ((nodes - 1) * nodes) / 2
    density = edges / max_edges
    print(f"\tDensity: {density}")
    print(
        "\tLargest weak component size: "
        f"{len(getLargestWeakComponent(undir_adj_list))}"
    )
    triangles = countTriangles(undir_adj_list)
    print(f"\ttriangles: {triangles // 3}")
    node_cluster_coeff = getNodeClusterCoeff(undir_adj_list, TEAM_NUMBER)
    print(f"\tNode {TEAM_NUMBER} cluster coefficient: {node_cluster_coeff}")
    print(
        f"\tDistance between {TEAM_NUMBER} and {nodes - 1}: "
        f"{getDistNodesPair(undir_adj_list, TEAM_NUMBER, nodes - 1)}"
    )


def processDirGraph(file_path: str) -> None:
    TEAM_NUMBER = 9
    print(f"Processing {file_path}")
    result = analyze_network(file_path, True)
    dir_adj_list = createDirAdjList(file_path)
    undir_adj_list = createUndirAdjList(file_path)
    nodes = result["vertices"]
    print(f"\tNodes: {result['vertices']}")
    print(f"\tEdges: {result['edges']}")
    diameter, radius = getGraphDiamAndRadius(dir_adj_list)
    print(f"\tDiameter: {diameter}")
    print(f"\tRadius: {radius}")
    print(f"\tDensity: {result['density']}")
    print(
        "\tLargest weak component size: "
        f"{len(getLargestWeakComponent(undir_adj_list))}"
    )
    triangles = countTriangles(undir_adj_list)
    print(f"\ttriangles: {triangles // 3}")

    node_cluster_coeff = getNodeClusterCoeff(dir_adj_list, TEAM_NUMBER)
    print(f"\tNode {TEAM_NUMBER} cluster coefficient: {node_cluster_coeff}")
    dist1 = getDistNodesPair(undir_adj_list, TEAM_NUMBER, nodes - 1)
    dist = dist1
    if dist1 == -1:
        dist = getDistNodesPair(undir_adj_list, nodes - 1, TEAM_NUMBER)
    print(f"\tDistance between {TEAM_NUMBER} and {nodes - 1}: {dist}")
    print(
        f"\tNumber of strongly connected components: {result['strong_components']}"
    )


# import networkx as nx
#
# undir_graphs = [
#     "nx_tests/graph_0.txt",
#     "nx_tests/graph_1.txt",
#     # "nx_tests/graph_2.txt",
#     # "nx_tests/graph_3.txt",
#     # "nx_tests/graph_4.txt",
#     "nx_tests/graph_5.txt",
#     "nx_tests/graph_6.txt",
#     "nx_tests/graph_7.txt",
#     "nx_tests/graph_8.txt",
# ]
#
# dir_graphs = [
#     "nx_tests/digraph_1.txt",
#     "nx_tests/digraph_2.txt",
#     "nx_tests/digraph_3.txt",
# ]
#
#
# def getGraphDiamAndRadius(
#     undir_adj_list: Tuple[Set[int], ...],
# ) -> Tuple[int, int]:
#     diameter = 0
#     radius = 10**9
#     total_nodes = len(undir_adj_list)
#     for node_ind in range(total_nodes):
#         _, new_distance = getFurthestNodeBFS(undir_adj_list, node_ind)
#         diameter = max(diameter, new_distance)
#         radius = min(radius, new_distance)
#     return (diameter, radius)
#
#
# def processUndirGraph(file_path: str) -> None:
#     TEAM_NUMBER = 9
#     print(f"Processing {file_path}")
#     undir_graph = nx.read_edgelist(file_path, nodetype=int)
#     dir_graph = nx.read_edgelist(
#         file_path, create_using=nx.DiGraph(), nodetype=int
#     )
#     nodes = undir_graph.number_of_nodes()
#     print(f"\tNodes: {undir_graph.number_of_nodes()}")
#     print(f"\tEdges: {undir_graph.number_of_edges()}")
#     print(f"\tDiameter: {nx.diameter(undir_graph)}")
#     print(f"\tRadius: {nx.radius(undir_graph)}")
#     print(f"\tDensity: {nx.density(undir_graph)}")
#     weak_components = nx.weakly_connected_components(dir_graph)
#     largest_weak_component_size = max(len(c) for c in weak_components)
#     print(f"\tLargest weak component size: {largest_weak_component_size}")
#     triangles_per_node = nx.triangles(undir_graph)
#     total_triangles = sum(triangles_per_node.values()) // 3
#     print(f"\ttriangles: {total_triangles // 3}")
#     print(
#         f"\tNode {TEAM_NUMBER} cluster coefficient: "
#         f"{nx.clustering(undir_graph, TEAM_NUMBER)}"
#     )
#     print(
#         f"\tDistance between {TEAM_NUMBER} and {nodes - 1}: "
#         f"{nx.shortest_path_length(undir_graph, TEAM_NUMBER, nodes - 1)}"
#     )
#
#
# def processDirGraph(file_path: str) -> None:
#     print(f"Processing {file_path}")
#     dir_graph = nx.read_edgelist(
#         file_path, create_using=nx.DiGraph(), nodetype=int
#     )
#     scc = list(nx.strongly_connected_components(dir_graph))
#     print("Number of strongly connected components:", len(scc))


if __name__ == "__main__":
    for graph_path in undir_graphs:
        processUndirGraph(graph_path)
    for graph_path in dir_graphs:
        processDirGraph(graph_path)
