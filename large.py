import random
import networkx as nx
from time import perf_counter
import numpy as np


def load_large_graph_from_file(filename: str, vertices: set):
    G = nx.Graph()
    edge_count = 0
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.replace(',', ' ').split()
            if len(parts) < 2:
                continue
            try:
                u, v = map(int, parts[:2])
            except ValueError:
                continue
            if u in vertices and v in vertices:
                if u != v:  # Проверка на петли
                    G.add_edge(u, v)
                    edge_count += 1
        for v in vertices:
            G.add_node(v)
    return nx.to_dict_of_lists(G), edge_count


def load_graph(filename: str, k: int = 50000):
    unique_vertices = set()
    degrees = dict()
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue  # Пропускаем комментарии и пустые строки
            parts = line.replace(',', ' ').split()
            try:
                u, v = map(int, parts[:2])
                unique_vertices.add(u)
                unique_vertices.add(v)
                if u != v:
                    degrees[u] = degrees.get(u, 0) + 1
                    degrees[v] = degrees.get(v, 0) + 1

            except ValueError:
                continue  # Пропускаем строки, где не удаётся распарсить числа
    for v in unique_vertices:
        if v not in degrees:
            degrees[v] = 0
    all_degrees = list(degrees.values())
    q1 = np.percentile(all_degrees, 25)
    q3 = np.percentile(all_degrees, 75)
    low = [v for v in degrees if degrees[v] <= q1]
    medium = [v for v in degrees if q1 < degrees[v] <= q3]
    high = [v for v in degrees if degrees[v] > q3]
    n_low = min(len(low), int(0.3 * k))
    n_medium = min(len(medium), int(0.4 * k))
    n_high = min(len(high), k - n_low - n_medium)
    sampled_vertices = set(random.sample(low, n_low) +
                           random.sample(medium, n_medium) +
                           random.sample(high, n_high))

    # sampled = random.sample(list(unique_vertices), min(k, real_length_vertices))
    graph, edge_count = load_large_graph_from_file(filename, set(sampled_vertices) )
    print("LOAD_GRAPH")
    # start = perf_counter()
    print(f"кол-во вершин реальное = {len(unique_vertices)}")
    # print(f"кол-во ребер =  {edge_count}")
    # density = 2 * edge_count / (k * (k - 1))
    # print(f"плотность = {density}")
    # end = perf_counter()
    # print(f"\n⏱ Время выполнения A1: {end - start:.6f} секунд")
    # print("(A1)----------\n")

    return graph #, degrees, len(unique_vertices)