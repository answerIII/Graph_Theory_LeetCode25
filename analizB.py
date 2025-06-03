import random
from collections import defaultdict, deque
import matplotlib.pyplot as plt
import os
import glob

def read_graph(path):
    graph = defaultdict(set)
    with open(path, 'r') as f:
        for line in f:
            u, v = map(int, line.strip().split())
            if u == v:
                continue
            graph[u].add(v)
            graph[v].add(u)
    return graph

def get_connected_components(graph):
    visited = set()
    components = []
    for node in graph:
        if node not in visited:
            comp = []
            queue = deque([node])
            visited.add(node)
            while queue:
                current = queue.popleft()
                comp.append(current)
                for neighbor in graph[current]:
                    if neighbor not in visited:
                        visited.add(neighbor)
                        queue.append(neighbor)
            components.append(comp)
    return components

def remove_nodes(graph, nodes_to_remove):
    new_graph = defaultdict(set)
    for u in graph:
        if u in nodes_to_remove:
            continue
        new_graph[u] = {v for v in graph[u] if v not in nodes_to_remove}
    return new_graph

def fragmentation_analysis(graph, percents):
    N = len(graph)
    original_nodes = set(graph.keys())
    degrees = {node: len(graph[node]) for node in graph}
    sorted_by_degree = sorted(degrees, key=degrees.get, reverse=True)
    random_results = []
    targeted_results = []
    for p in percents:
        num_remove = int(N * p / 100)
        to_remove_rand = set(random.sample(list(original_nodes), num_remove))
        g_rand = remove_nodes(graph, to_remove_rand)
        comps_rand = get_connected_components(g_rand)
        max_size_rand = max((len(c) for c in comps_rand), default=0)
        random_results.append(max_size_rand / (N - num_remove))
        to_remove_deg = set(sorted_by_degree[:num_remove])
        g_deg = remove_nodes(graph, to_remove_deg)
        comps_deg = get_connected_components(g_deg)
        max_size_deg = max((len(c) for c in comps_deg), default=0)
        targeted_results.append(max_size_deg / (N - num_remove))
    return random_results, targeted_results

def plot_fragmentation(percents, rand_results, deg_results):
    plt.plot(percents, rand_results, marker='o', label='Случайное удаление')
    plt.plot(percents, deg_results, marker='s', label='Удаление по степени')
    plt.xlabel("Удалено x% узлов")
    plt.ylabel("Доля вершин в наибольшей компоненте")
    plt.title("Устойчивость графа к удалению узлов")
    plt.legend()
    plt.show()

txt_files = sorted(glob.glob(os.path.join("*.txt")))
for i, file in enumerate(txt_files, 1):
    print(f"\n[{i}/{len(txt_files)}] Файл: {os.path.basename(file)}")
    graph = read_graph(os.path.basename(file))
    percents = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50]
    rand_res, deg_res = fragmentation_analysis(graph, percents)
    for p, r, d in zip(percents, rand_res, deg_res):
        print(f"{p}%: случайное = {r}, по степени = {d}")
    plot_fragmentation(percents, rand_res, deg_res)