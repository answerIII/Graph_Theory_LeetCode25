import networkx as nx

file_path = 'Email-EuAll.txt'
G = nx.DiGraph()
with open(file_path, 'r') as f:
    for line in f:
        u, v = map(int, line.strip().split())
        G.add_edge(u, v)

num_nodes = G.number_of_nodes()
num_edges = G.number_of_edges()
max_possible_edges = num_nodes * (num_nodes - 1) / 2
density = num_edges / max_possible_edges
components = list(nx.weakly_connected_components(G))
num_components = len(components)
largest_component_size = max(len(c) for c in components)
largest_component_ratio = largest_component_size / num_nodes
stcomponents = list(nx.strongly_connected_components(G))
stnum_components = len(stcomponents)
stlargest_component_size = max(len(c) for c in stcomponents)
stlargest_component_ratio = stlargest_component_size / num_nodes
print(f"Число вершин: {num_nodes}")
print(f"Число рёбер: {num_edges}")
print(f"Плотность графа: {density}")
print(f"Число компонент слабой связанности связности: {num_components}")
print(f"Доля вершин в наибольшей компоненте слабой связанности: {largest_component_ratio}")
print(f"Число компонент сильной связанности связности: {stnum_components}")
print(f"Доля вершин в наибольшей компоненте сильной связанности: {stlargest_component_ratio}")
