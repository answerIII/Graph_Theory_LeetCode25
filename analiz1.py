from collections import defaultdict, deque
import os
import glob

def read_directed_graph(file_path):
    graph = defaultdict(list)
    with open(file_path, 'r') as f:
        for line in f:
            u, v = map(int, line.strip().split())
            graph[u].append(v)
            if v not in graph:
                graph[v] = []
    return graph

def build_undirected(graph):
    undirected = defaultdict(set)
    for u in graph:
        for v in graph[u]:
            undirected[u].add(v)
            undirected[v].add(u)
        if u not in undirected:
            undirected[u] = set()
    return undirected

def weakly_connected_components(graph):
    undirected = build_undirected(graph)
    visited = set()
    components = []
    for node in undirected:
        if node not in visited:
            queue = deque([node])
            component = []
            while queue:
                v = queue.popleft()
                if v not in visited:
                    visited.add(v)
                    component.append(v)
                    queue.extend(undirected[v] - visited)
            components.append(component)
    return components

def reverse_graph(graph):
    reversed_g = defaultdict(list)
    for u in graph:
        for v in graph[u]:
            reversed_g[v].append(u)
        if u not in reversed_g:
            reversed_g[u] = []
    return reversed_g

def dfs_fill(graph, node, visited, order):
    stack = [node]
    temp_stack = []
    while stack:
        v = stack.pop()
        if v not in visited:
            visited.add(v)
            temp_stack.append(v)
            stack.extend([nei for nei in graph[v] if nei not in visited])
    while temp_stack:
        order.append(temp_stack.pop())

def dfs_collect(graph, node, visited, component):
    stack = [node]
    while stack:
        v = stack.pop()
        if v not in visited:
            visited.add(v)
            component.append(v)
            stack.extend([nei for nei in graph[v] if nei not in visited])

def kosaraju_scc(graph):
    visited = set()
    order = []
    for node in graph:
        if node not in visited:
            dfs_fill(graph, node, visited, order)
    reversed_g = reverse_graph(graph)
    visited.clear()
    components = []
    while order:
        node = order.pop()
        if node not in visited:
            component = []
            dfs_collect(reversed_g, node, visited, component)
            components.append(component)
    return components

txt_files = sorted(glob.glob(os.path.join("*.txt")))
for i, file in enumerate(txt_files, 1):
    print(f"\n[{i}/{len(txt_files)}] Файл: {os.path.basename(file)}")
    graph = read_directed_graph(os.path.basename(file))
    num_vertices = len(graph)
    num_edges = sum(len(neighbors) for neighbors in graph.values())
    max_edges = num_vertices * (num_vertices - 1) // 2
    density = num_edges / max_edges
    weak_components = weakly_connected_components(graph)
    num_weak = len(weak_components)
    largest_weak = max(len(comp) for comp in weak_components)
    weak_fraction = largest_weak / num_vertices
    strong_components = kosaraju_scc(graph)
    num_strong = len(strong_components)
    largest_strong = max(len(comp) for comp in strong_components)
    strong_fraction = largest_strong / num_vertices
    print(f"Число вершин: {num_vertices}")
    print(f"Число рёбер: {num_edges}")
    print(f"Плотность: {density}")
    print(f"Число компонент слабой связности: {num_weak}")
    print(f"Доля вершин в крупнейшей компоненте слабой компоненте: {weak_fraction}")
    print(f"Число компонент сильной связности: {num_strong}")
    print(f"Доля вершин в крупнейшей компоненте сильной компоненте: {strong_fraction}")
