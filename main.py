from basic import load_graph_from_file
from basic import load_large_graph_from_file

from analysis import *
from distance import *
def main():
    directed: bool =  False
    G = load_graph_from_file("data/undirected/CA-GrQc.txt", directed)
    
    #для больших графов
    #G = load_large_graph_from_file("data/very_large_graphs/")
    
    print("Section 1------\n")
    print_analysis(G, directed)
    print("\nSection 1------\n\n")
    print("Section 2------\n")
    while True:
        user_input = input( """Введите какой метод использовать для выбора ориентиров:  (для остановки введите слово 'stop')\n 1. random_selection\n 2. highest_degree_selection\n 3. best_coverage_selection\n""")
        if user_input.lower() == 'stop':
            break
        print_distance(G, directed, int(user_input))
    print("\nSection 2------\n\n")

if __name__ == "__main__":
    main()