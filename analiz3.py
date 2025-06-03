from collections import defaultdict
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

def count_triangles(graph):
    triangle_count = 0
    for u in graph:
        for v in graph[u]:
            if v > u:
                common = graph[u] & graph[v]
                triangle_count = triangle_count + len([w for w in common if w > v])
    return triangle_count

def global_clustering_coefficient(graph, triangle_count):
    triples = 0
    for v in graph:
        deg = len(graph[v])
        if deg >= 2:
            triples = triples + deg * (deg - 1) // 2
    return (3 * triangle_count / triples)

def average_clustering_coefficient(graph):
    total = 0
    count = 0
    for node in graph:
        neighbors = graph[node]
        k = len(neighbors)
        if k < 2:
            continue
        links = 0
        neighbors_list = list(neighbors)
        for i in range(k):
            for j in range(i + 1, k):
                u, v = neighbors_list[i], neighbors_list[j]
                if u in graph[v]:
                    links = links + 1
        total = total + (2 * links) / (k * (k - 1))
        count = count + 1
    return total / count

txt_files = sorted(glob.glob(os.path.join("*.txt")))
for i, file in enumerate(txt_files, 1):
    print(f"\n[{i}/{len(txt_files)}] Файл: {os.path.basename(file)}")
    graph = read_graph(os.path.basename(file))
    tri_count = count_triangles(graph)
    print(f"Число треугольников: {tri_count}")
    gcc = global_clustering_coefficient(graph, tri_count)
    print(f"Глобальный кластерный коэффициент: {gcc}")
    acc = average_clustering_coefficient(graph)
    print(f"Средний кластерный коэффициент: {acc}")