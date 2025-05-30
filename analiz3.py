import networkx as nx

def clustering_analysis(G):
    triangle_count = sum(nx.triangles(G).values()) // 3
    avg_clustering = nx.average_clustering(G)
    global_clustering = nx.transitivity(G)

    print(f"Число треугольников: {triangle_count}")
    print(f"Средний кластерный коэффициент: {avg_clustering}")
    print(f"Глобальный кластерный коэффициент: {global_clustering}")

file_path = 'Email-EuAll.txt'
G = nx.Graph()
with open(file_path, 'r') as f:
    for line in f:
        u, v = map(int, line.strip().split())
        G.add_edge(u, v)
clustering_analysis(G)