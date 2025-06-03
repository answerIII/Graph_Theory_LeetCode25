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
    lcc = get_largest_connected_component(graph)
    avg_c = average_clustering_coefficient(lcc)
    print(f"Средний кластерный коэффициент в наибольшей компоненте связанности: {avg_c}")