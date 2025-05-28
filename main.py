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
    #G = load_graph_from_file("data/directed/Wiki-Vote.txt", directed)
    
    #для больших графов
    #G = load_large_graph_from_file("data/very_large_graphs/")
    G = load_graph("data/very_large_graphs/very_large_graphs/com-orkut.ungraph.txt")
    print("Section 1------\n")
    comp = print_analysis(G, directed, large=False)
    print("\nSection 1------\n\n")
    print("Section 2------\n")
    subgraph = extract_subgraph(G, comp)
    while True:
        user_input = input( """Введите какой метод использовать для выбора ориентиров:  (для остановки введите слово 'stop')\n 1. random_selection\n 2. highest_degree_selection\n 3. best_coverage_selection\n""")
        if user_input.lower() == 'stop':
            break
        print_distance(subgraph, directed, int(user_input))
    print("\nSection 2------\n\n")

if __name__ == "__main__":
    main()