import networkx as nx
import random
import numpy as np

def double_sweep_diameter(G):
    start = random.choice(list(G.nodes))
    lengths = nx.single_source_shortest_path_length(G, start)
    a = max(lengths, key=lengths.get)
    lengths_a = nx.single_source_shortest_path_length(G, a)
    b = max(lengths_a, key=lengths_a.get)
    diameter_estimate = lengths_a[b]
    return diameter_estimate

def sample_distances(G, sample_size=500):
    nodes = list(G.nodes)
    distances = []
    for _ in range(sample_size):
        u, v = random.sample(nodes, 2)
        d = nx.shortest_path_length(G, u, v)
        distances.append(d)
    return np.percentile(distances, 90)

def snowball_sample(G, size_target=500):
    seed_nodes = random.sample(list(G.nodes), 2)
    visited = set(seed_nodes)
    frontier = set(seed_nodes)
    while len(visited) < size_target and frontier:
        next_frontier = set()
        for node in frontier:
            neighbors = set(G.neighbors(node))
            next_frontier.update(neighbors - visited)
        visited.update(next_frontier)
        frontier = next_frontier
    return G.subgraph(visited)

file_path = 'Email-EuAll.txt'
G = nx.Graph()
with open(file_path, 'r') as f:
    for line in f:
        u, v = map(int, line.strip().split())
        G.add_edge(u, v)
components = list(nx.connected_components(G))
largest_cc = max(components, key=len)
G_lcc = G.subgraph(largest_cc).copy()
ds_diameter = double_sweep_diameter(G_lcc)
perc90 = sample_distances(G_lcc, 500)
snowball = snowball_sample(G_lcc, 500)
print("Наибольшей компоненты слабой связанности состоит из {} вершин:".format(G_lcc.number_of_nodes()))
print(f"Диаметр по double sweep: {ds_diameter}")
print(f"90-й перцентиль расстояний между случайными вершинами: {perc90}")
if snowball.number_of_nodes() >= 2:
    sb_diameter = double_sweep_diameter(snowball)
    sb_perc90 = sample_distances(snowball, 500)
    print(f"Диаметр снежного кома: {sb_diameter}")
    print(f"90-й перцентиль снежного кома: {sb_perc90}")
else:
    print("Не удалось построить снежный ком из 500 узлов")
