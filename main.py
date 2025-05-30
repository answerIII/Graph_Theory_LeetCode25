from basic import load_graph_from_file
from basic import load_large_graph_from_file
from large import load_graph
from analysis import *
from distance import *

def extract_subgraph(full_graph, vertices):
    """Формирует подграф из множества вершин."""
    return {v: [u for u in neighbors if u in vertices]
            for v, neighbors in full_graph.items()
            if v in vertices}

def main():
    directed: bool =  False
    G, library_graph = load_graph_from_file("data/undirected/CA-GrQc.txt", directed)#!!!используем это для обычных
    
    #для больших графов
    #G = load_large_graph_from_file("data/very_large_graphs/") старая версия

    print("Section 1------\n")
    #G, library_graph = load_graph("data/undirected/Email-EuAll.txt")   #!!!!!используем это для больших

    comp = print_analysis(G, directed, library_graph)
    print("\nSection 1------\n\n")
    print("Section 2------\n")
    subgraph = {v: {n for n in G[v] if n in comp} for v in comp}

    while True:
        user_input = input( """Введите какой метод использовать для выбора ориентиров:  (для остановки введите слово 'stop')\n 1. random_selection\n 2. highest_degree_selection\n 3. best_coverage_selection\n""")
        if user_input.lower() == 'stop':
            break
    
        print_distance(subgraph, int(user_input))
    print("\nSection 2------\n\n")

if __name__ == "__main__":
    main()