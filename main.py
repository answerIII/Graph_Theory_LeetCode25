from basic import load_graph_from_file
from analysis import *
from distance import *
def main():
    directed: bool = True
    G, graph_with_correct_result = load_graph_from_file("data/example_graph.txt", directed)
    print("Section 1------\n\n")
    print_analysis(G, directed, graph_with_correct_result)
    print("\n\nSection 1------\n\n")
    print_distance(G, directed)

if __name__ == "__main__":
    main()