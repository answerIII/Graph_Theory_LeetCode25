import networkx as nx

def clustering_analysis(G):
    avg_clustering = nx.average_clustering(G)
    print(f"Средний кластерный коэффициент наибольшей компоненты слабой связанности: {avg_clustering}")

file_path = 'Email-EuAll.txt'
G = nx.Graph()
with open(file_path, 'r') as f:
    for line in f:
        u, v = map(int, line.strip().split())
        G.add_edge(u, v)
G_lcc = G.subgraph(max(nx.connected_components(G), key=len)).copy()
clustering_analysis(G_lcc)