import random
import numpy as np
from collections import defaultdict, deque
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

def bfs_distances(graph, start):
    distances = {start: 0}
    queue = deque([start])
    while queue:
        node = queue.popleft()
        for neighbor in graph[node]:
            if neighbor not in distances:
                distances[neighbor] = distances[node] + 1
                queue.append(neighbor)
    return distances

def get_largest_connected_component(graph):
    visited = set()
    largest_component = []
    nodes = list(graph.keys())
    for node in nodes:
        if node in visited:
            continue
        component = []
        queue = deque([node])
        visited.add(node)
        while queue:
            current = queue.popleft()
            component.append(current)
            for neighbor in graph[current]:
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append(neighbor)
        if len(component) > len(largest_component):
            largest_component = component
    component_set = set(largest_component)
    subgraph = {v: graph[v] & component_set for v in component_set}
    return subgraph

def double_sweep_diameter(graph):
    r = random.choice(list(graph.keys()))
    dist1 = bfs_distances(graph, r)
    a = max(dist1, key=dist1.get)
    dist2 = bfs_distances(graph, a)
    b = max(dist2, key=dist2.get)
    return dist2[b]

def sample_distances(graph, sources=10, targets_per_source=50):
    nodes = list(graph.keys())
    distances = []
    for _ in range(sources):
        src = random.choice(nodes)
        dist_map = bfs_distances(graph, src)
        others = [v for v in dist_map if v != src]
        if not others:
            continue
        sampled = random.sample(others, min(targets_per_source, len(others)))
        distances.extend(dist_map[v] for v in sampled)
    return np.percentile(distances, 90)

def snowball_sample(graph, target_size=500, max_expand=200):
    seed = random.sample(list(graph.keys()), 2)
    visited = set(seed)
    queue = deque(seed)
    while queue and len(visited) < target_size:
        node = queue.popleft()
        neighbors = graph[node] - visited
        if len(neighbors) > max_expand:
            neighbors = random.sample(list(neighbors), max_expand)
        for neigh in neighbors:
            visited.add(neigh)
            queue.append(neigh)
            if len(visited) >= target_size:
                break
    return {v: graph[v] & visited for v in visited}

txt_files = sorted(glob.glob(os.path.join("*.txt")))
for i, file in enumerate(txt_files, 1):
    print(f"\n[{i}/{len(txt_files)}] Файл: {os.path.basename(file)}")
    graph = read_graph(os.path.basename(file))
    largest_cc = get_largest_connected_component(graph)
    diameter = double_sweep_diameter(largest_cc)
    print("Оценка диаметра (double sweep):", diameter)
    perc90 = sample_distances(largest_cc)
    print("90-й перцентиль расстояний:", round(perc90, 2))
    snowball = snowball_sample(largest_cc, 500)
    if len(snowball) >= 2:
        sb_diameter = double_sweep_diameter(snowball)
        sb_perc90 = sample_distances(snowball)
        print("Диаметр снежного кома (double sweep):", sb_diameter)
        print("90-й перцентиль снежного кома:", round(sb_perc90, 2))
    else:
        print("Снежный ком из 500 вершин не построен :(")