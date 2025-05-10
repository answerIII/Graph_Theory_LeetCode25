from graph import Graph

if __name__ == "__main__":
    graph: 'Graph' = Graph.from_file('directed/test.txt')
    print(graph.find_scc())