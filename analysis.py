from A1 import *
from basic import to_undirected
import networkx as nx

"""тут когда все методы для пункта 1 сделаем напишем код который соберет все и выведет анализ графа"""
def print_analysis(graph: dict, directed: bool, graph_with_correct_result: nx.Graph):
    num_of_vertices = number_of_vertices(graph)
    num_of_edges = number_of_edges(graph, directed)
    print(f"кол-во вершин = {num_of_vertices} (должно быть {graph_with_correct_result.number_of_nodes()})")
    print(f"кол-во вершин =  {num_of_edges} (должно быть {graph_with_correct_result.number_of_edges()})")
    print(f"плотность = {density(num_of_edges, num_of_vertices)} (должно быть {nx.density(graph_with_correct_result)})")
    week_max_component : set  # самая большая компонента по количеству вершин
    week_component_count: int
    if directed:
        week_component_count, week_max_component = weekly_connected_components(to_undirected(graph))
        correct_components = len(list(nx.weakly_connected_components(graph_with_correct_result)))

    else:
        week_component_count, week_max_component = weekly_connected_components(graph)
        correct_components = len(list(nx.connected_components(graph_with_correct_result)))

    
    print(f"кол-во комп. слабой свзяности = {week_component_count} (должно быть {correct_components})")
    print(f"дол в вершин в макс. по мощности компоненте = {len(week_max_component)/num_of_vertices}")
    
