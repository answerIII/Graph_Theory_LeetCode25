from basic import load_graph_from_file
from analysis import *
from distance import *
def main():
    directed: bool = True
    G, graph_with_correct_result = load_graph_from_file("data/undirected/ca-coauthors-dblp.txt", directed)
    print("Section 1------\n")
    print_analysis(G, directed, graph_with_correct_result)
    print("\nSection 1------\n\n")
    print("Section 2------\n")
    while True:
        user_input = input( """Введите какой метод использовать для выбора ориентиров:  (для остановки введите слово 'stop')\n 1. random_selection\n 2. highest_degree_selection\n 3. best_coverage_selection""")
        if user_input.lower() == 'stop':
            break
        print_distance(G, directed, int(user_input))
    print("\nSection 1------\n\n")

if __name__ == "__main__":
    main()