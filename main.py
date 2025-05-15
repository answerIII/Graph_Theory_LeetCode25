from basic import load_graph_from_file
from analysis import *
def main():
    directed: bool = False
    G = load_graph_from_file("data/undirected/CA-AstroPh.txt", directed)
    print_analysis(G, directed)



if __name__ == "__main__":
    main()