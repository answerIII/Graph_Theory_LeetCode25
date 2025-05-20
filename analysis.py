from A1 import *
from A2 import *
from A3 import *
from A4 import *
from B import *

from basic import to_undirected
import networkx as nx

"""тут когда все методы для пункта 1 сделаем напишем код который соберет все и выведет анализ графа"""
def print_analysis(graph: dict, directed: bool, graph_with_correct_result: nx.Graph):
    num_of_vertices = number_of_vertices(graph)
    num_of_edges = number_of_edges(graph, directed)
    print("(A1)----------")
    print(f"кол-во вершин = {num_of_vertices} (должно быть {graph_with_correct_result.number_of_nodes()})")
    print(f"кол-во вершин =  {num_of_edges} (должно быть {graph_with_correct_result.number_of_edges()})")
    print(f"плотность = {density(num_of_edges, num_of_vertices)} (должно быть {nx.density(graph_with_correct_result)})")
    week_max_component : set  # самая большая компонента по количеству вершин
    week_component_count: int
    undirected_graph = to_undirected(graph) if directed else graph

    if directed:
        week_component_count, week_max_component = weekly_connected_components(to_undirected(graph))
        correct_components = len(list(nx.weakly_connected_components(graph_with_correct_result)))

    else:
        week_component_count, week_max_component = weekly_connected_components(graph)
        correct_components = len(list(nx.connected_components(graph_with_correct_result)))

    fraction_of_vertices_largest_week_component = len(week_max_component)/num_of_vertices
    print(f"кол-во комп. слабой свзяности = {week_component_count} (должно быть {correct_components})")
    print(f"дол в вершин в макс. по мощности слабой компоненте = {fraction_of_vertices_largest_week_component}")
    if directed:
        count, strong_max_comp_length = strongly_connected_components(graph)
        print(f"кол-во компонент сильной свзяности = {count} (должно быть {len(list(nx.strongly_connected_components(graph_with_correct_result)))})")
        print(f"доля вершин графа в наибольшей компоненте сильной связности = {strong_max_comp_length/num_of_vertices}")
    print("(A1)----------\n")

    print("(A2)----------")
    print(f"диаметр Double sweep = {double_sweep(undirected_graph, week_max_component)}")    
    print(f"диаметр Random = {random_pairwise_distances(undirected_graph, week_max_component)}")
    print(f"диаметр Snowball sample = {snowball_sampling(undirected_graph, week_max_component)}")
    print("(A2)----------\n")

    print("(A3)----------")
    print(f"число треугольников = {count_triangles(undirected_graph)}")
    print(f"средний кластерный коэффициент = {average_clustering(undirected_graph)}")
    print(f"глобальный кластерный коэффициент = {global_clustering(undirected_graph)}")
    print("(A3)----------\n")

    print("(A4)----------")
    print(f"средний кластерный коэффициент (для наибольшей компоненты слабой связанности) = {average_clustering_coefficient(undirected_graph)}")
    print("(A4)----------\n")

    print("(B1)----------")
    if directed:
        graph = to_undirected(graph)
    while True:
        user_input = input("Введите процент рандомных вершин который будет удален (для остановки введите слово 'stop')\n")
        graph_with_removed_vertices = {node: set(neighbors) for node, neighbors in graph.items()}
        if user_input.lower() == 'stop':
            break
        remove_random_vertices(graph_with_removed_vertices, int(user_input))
        count, max_component = weekly_connected_components(graph_with_removed_vertices)
        fraction_after_remove = len(max_component)/ len(graph_with_removed_vertices)
        print(f"доля вершин в наибольшей компоненте\nдо удаления {fraction_of_vertices_largest_week_component}\nпосле  {fraction_after_remove}")
        print("до удаления вершин:", len(graph))
        print("после удаления вершин:", len(graph_with_removed_vertices))
        print("размер наибольшей компоненты:", len(max_component))
    print("(B1)----------\n")
    print("\n(B2)----------")
    while True:
        user_input = input("Введите процент вершин наибольшей степени который будет удален  (для остановки введите слово 'stop')\n")
        graph_with_removed_vertices = {node: set(neighbors) for node, neighbors in graph.items()}
        if user_input.lower() == 'stop':
            break
        remove_top_degree_vertices(graph_with_removed_vertices, int(user_input))
        count, max_component = weekly_connected_components(graph_with_removed_vertices)
        fraction_after_remove = len(max_component)/ len(graph_with_removed_vertices)
        print(f"доля вершин в наибольшей компоненте\nдо удаления {fraction_of_vertices_largest_week_component}\nпосле  {fraction_after_remove}")
        print("до удаления вершин:", len(graph))
        print("после удаления вершин:", len(graph_with_removed_vertices))
        print("размер наибольшей компоненты:", len(max_component))
    print("\n(B2)----------")
