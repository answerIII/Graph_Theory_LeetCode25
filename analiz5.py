import matplotlib.pyplot as plt
from collections import defaultdict, Counter
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

def degree_stats(graph):
    degrees = [len(neighbors) for neighbors in graph.values()]
    min_deg = min(degrees)
    max_deg = max(degrees)
    avg_deg = sum(degrees) / len(degrees)
    return degrees, min_deg, max_deg, avg_deg

def degree_distribution(degrees):
    counter = Counter(degrees)
    total = sum(counter.values())
    pdf = {deg: count / total for deg, count in counter.items()}
    return pdf

def plot_degree_distribution(pdf):
    x = sorted(pdf.keys())
    y = [pdf[d] for d in x]
    plt.subplot(1, 2, 1)
    plt.plot(x, y, marker='o')
    plt.title("Распределение степеней")
    plt.xlabel("Степень")
    plt.ylabel("P(k)")
    plt.subplot(1, 2, 2)
    plt.loglog(x, y, marker='o', linestyle='None')
    plt.title("Log-Log распределение")
    plt.xlabel("log(Степень)")
    plt.ylabel("log(P(k))")
    plt.show()

txt_files = sorted(glob.glob(os.path.join("*.txt")))
for i, file in enumerate(txt_files, 1):
    print(f"\n[{i}/{len(txt_files)}] Файл: {os.path.basename(file)}")
    graph = read_graph(os.path.basename(file))
    degrees, dmin, dmax, davg = degree_stats(graph)
    print(f"Мин. степень: {dmin}")
    print(f"Макс. степень: {dmax}")
    print(f"Средняя степень: {davg}")
    pdf = degree_distribution(degrees)
    plot_degree_distribution(pdf)