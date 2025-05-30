import networkx as nx
import matplotlib.pyplot as plt
from collections import Counter
import numpy as np

def plot_degree_distribution(G):
    degrees = [deg for _, deg in G.degree()]
    min_deg = min(degrees)
    max_deg = max(degrees)
    avg_deg = sum(degrees) / len(degrees)
    print(f"Минимальная степень: {min_deg}")
    print(f"Максимальная степень: {max_deg}")
    print(f"Средняя степень: {avg_deg}")
    degree_counts = Counter(degrees)
    total_nodes = G.number_of_nodes()
    x = np.array(sorted(degree_counts.keys()))
    y = np.array([degree_counts[d] / total_nodes for d in x])
    plt.plot(x, y, marker='o', linestyle='-', color='blue')
    plt.title("Распределение степеней узлов, обычная шкала")
    plt.xlabel("Степень узла")
    plt.ylabel("Вероятность")
    plt.show()
    plt.loglog(x, y, marker='o', linestyle='-', color='red')
    plt.title("Распределение степеней узлов, log-log шкала")
    plt.xlabel("log(Степень узла)")
    plt.ylabel("log(Вероятность)")
    plt.show()

file_path = 'Email-EuAll.txt'
G = nx.Graph()
with open(file_path, 'r') as f:
    for line in f:
        u, v = map(int, line.strip().split())
        G.add_edge(u, v)
G_lcc = G.subgraph(max(nx.connected_components(G), key=len)).copy()
plot_degree_distribution(G_lcc)
