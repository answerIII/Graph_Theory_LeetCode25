from basic import load_graph_from_file
from analysis import *
def main():
    directed: bool = False
    G, graph_with_correct_result = load_graph_from_file("data/undirected/CA-AstroPh.txt", directed)
    print_analysis(G, directed, graph_with_correct_result)



if __name__ == "__main__":
    main()