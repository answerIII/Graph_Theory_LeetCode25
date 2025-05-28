import random
import networkx as nx
from time import perf_counter


def load_large_graph_from_file(filename: str, vertices: set):
    G = nx.Graph()
    edge_count = 0
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue  # пропускаем комментарии и пустые строки
            parts = line.replace(',', ' ').split()
            if len(parts) < 2:
                continue
            try:
                u, v = map(int, parts[:2])
            except ValueError:
                continue
            if u in vertices and v in vertices:
                G.add_edge(u, v)
                edge_count += 1
    return nx.to_dict_of_lists(G), edge_count


def load_graph(filename: str, k: int = 50000):
    unique_vertices = set()
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue  # Пропускаем комментарии и пустые строки
            try:
                u, v = map(int, line.split())
                unique_vertices.add(u)
                unique_vertices.add(v)
            except ValueError:
                continue  # Пропускаем строки, где не удаётся распарсить числа
    real_length_vertices = len(unique_vertices)
    sampled = random.sample(list(unique_vertices), min(k, real_length_vertices))
    vertex_dict = {v: [] for v in sampled}

    graph, edge_count = load_large_graph_from_file(filename, vertex_dict)
    print("(A1)----------")
    start = perf_counter()
    print(f"кол-во вершин = {real_length_vertices}")
    print(f"кол-во ребер =  {edge_count}")
    print(f"плотность = {2 * edge_count / (real_length_vertices * (real_length_vertices - 1))}")
    # week_max_component : set  # самая большая компонента по количеству вершин
    # week_component_count: int
    # week_component_count, week_max_component = weekly_connected_components(graph)

    # fraction_of_vertices_largest_week_component = len(week_max_component)/num_of_vertices
    # print(f"кол-во комп. слабой свзяности = {week_component_count}")
    # print(f"дол в вершин в макс. по мощности слабой компоненте = {fraction_of_vertices_largest_week_component}")
    end = perf_counter()
    print(f"\n⏱ Время выполнения A1: {end - start:.6f} секунд")
    print("(A1)----------\n")

    return graph