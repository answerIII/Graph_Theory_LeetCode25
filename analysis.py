from A1 import *

"""тут когда все методы для пункта 1 сделаем напишем код который соберет все и выведет анализ графа"""
def print_analysis(graph: dict, directed: bool):
    num_of_vertices = number_of_vertices(graph)
    num_of_edges = number_of_edges(graph, directed)
    print(num_of_vertices)
    print(num_of_edges)
    print(density(num_of_edges, num_of_vertices))