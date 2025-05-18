from basic import load_graph_from_file
from analysis import *
def main():
    directed: bool = True
    G, graph_with_correct_result = load_graph_from_file("data/example_graph.txt", directed)
    print_analysis(G, directed, graph_with_correct_result)



if __name__ == "__main__":
    main()