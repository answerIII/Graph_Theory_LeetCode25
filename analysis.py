from A1 import *
from A2 import *
from A3 import *
from A4 import *
from A5 import *
from B import *
from basic import to_undirected
import networkx as nx
import matplotlib
import matplotlib.pyplot as plt
matplotlib.use('TkAgg')  # или 'Qt5Agg'
import math
from time import perf_counter

def plot_comparisons(stats1, stats2):
    #Сортируем по проценту
    stats1.sort(key=lambda x: x['percent'])
    stats2.sort(key=lambda x: x['percent'])

    percent1 = [s['percent'] for s in stats1]
    percent2 = [s['percent'] for s in stats2]

    fraction_after_1 = [s['fraction_after'] for s in stats1]
    fraction_after_2 = [s['fraction_after'] for s in stats2]

    damage1 = [s['fraction_before'] - s['fraction_after'] for s in stats1]
    damage2 = [s['fraction_before'] - s['fraction_after'] for s in stats2]


    plt.figure(figsize=(10, 5))
    plt.plot(percent1, fraction_after_1, marker='o', label='B1 - случайные')
    plt.plot(percent2, fraction_after_2, marker='s', label='B2 - по степени')
    plt.xlabel('Процент удалённых вершин')
    plt.ylabel('Доля вершин в наибольшей компоненте')
    plt.title('Сравнение связности после удаления')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


    plt.figure(figsize=(10, 5))
    plt.plot(percent1, damage1, marker='o', label='B1 - случайные')
    plt.plot(percent2, damage2, marker='s', label='B2 - по степени')
    plt.xlabel('Процент удалённых вершин')
    plt.ylabel(' Насколько сильно уменьшилась большая компонента')
    plt.title('Насколько уменьшилась связность')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

"""тут когда все методы для пункта 1 сделаем напишем код который соберет все и выведет анализ графа"""
def print_analysis(graph: dict, directed: bool):
    num_of_vertices = number_of_vertices(graph)
    num_of_edges = number_of_edges(graph, directed)
    
    print("(A1)----------")
    start = perf_counter()
    print(f"кол-во вершин = {num_of_vertices}")
    print(f"кол-во ребер =  {num_of_edges}")
    print(f"плотность = {density(num_of_edges, num_of_vertices, directed)}")
    week_max_component : set  # самая большая компонента по количеству вершин
    week_component_count: int
    undirected_graph = to_undirected(graph) if directed else graph

    if directed:
        week_component_count, week_max_component = weekly_connected_components(undirected_graph)

    else:
        week_component_count, week_max_component = weekly_connected_components(graph)

    fraction_of_vertices_largest_week_component = len(week_max_component)/num_of_vertices
    print(f"кол-во комп. слабой свзяности = {week_component_count}")
    print(f"дол в вершин в макс. по мощности слабой компоненте = {fraction_of_vertices_largest_week_component}")

    if directed:
        count, strong_max_comp_length = strongly_connected_components(graph)
        print(f"кол-во компонент сильной свзяности = {count} )")
        print(f"доля вершин графа в наибольшей компоненте сильной связности = {strong_max_comp_length/num_of_vertices}")
    else:
        undirected_graph = graph
    end = perf_counter()
    print(f"\n⏱ Время выполнения A1: {end - start:.6f} секунд")
    print("(A1)----------\n")


    print("(A2)----------")
    start = perf_counter()
    
    if (num_of_vertices < 300000):
        print(f"диаметр Double sweep = {double_sweep(undirected_graph, week_max_component)}")    
        print(f"диаметр Random = {random_pairwise_distances(undirected_graph, week_max_component)}")
    print(f"диаметр Snowball sample = {snowball_sampling(undirected_graph, week_max_component)}")
    end = perf_counter()
    print(f"\n⏱ Время выполнения A2: {end - start:.6f} секунд")
    print("(A2)----------\n")

    print("(A3)----------")
    start = perf_counter()
    print(f"число треугольников = {count_triangles(undirected_graph)}")
    print(f"средний кластерный коэффициент = {average_clustering(undirected_graph)}")
    print(f"глобальный кластерный коэффициент = {global_clustering(undirected_graph)}")
    end = perf_counter()
    print(f"\n⏱ Время выполнения A3: {end - start:.6f} секунд")    
    print("(A3)----------\n")

    print("(A4)----------")
    start = perf_counter()
    print(f"средний кластерный коэффициент (для наибольшей компоненты слабой связанности) = {average_clustering_coefficient(undirected_graph)}")
    end = perf_counter()
    print(f"\n⏱ Время выполнения A4: {end - start:.6f} секунд")
    print("(A4)----------\n")
    if directed:
        graph = undirected_graph
    print("(A5)----------")
    start = perf_counter()
    min_deg, max_deg, avg_deg,degree_prob = degree_stats_and_distribution(graph)
    end = perf_counter()

    print(f"Минимальная степень: {min_deg}")
    print(f"Максимальная степень: {max_deg}")
    print(f"Средняя степень: {avg_deg:.4f}")

    # 95-й персентиль
    degrees = np.array(list(degree_prob.keys()))
    p95 = np.percentile(degrees, 95)
    filtered_degree_prob = dict()
    for k, v in degree_prob.items():
        if k <= p95:
            filtered_degree_prob[k] = v
    plt.figure(figsize=(15, 5))
    plt.subplot(1, 3, 1)
    plt.bar(filtered_degree_prob.keys(), filtered_degree_prob.values(), width=0.80, color='b')
    plt.title("Распределение степеней (95-й персентиль)")
    plt.xlabel("Степень")
    plt.ylabel("Вероятность")

    # оригинальные данные
    plt.subplot(1, 3, 2)
    plt.bar(degree_prob.keys(), degree_prob.values(), width=0.80, color='b')
    plt.title("Распределение степеней (обычная шкала)")
    plt.xlabel("Степень")
    plt.ylabel("Вероятность")

    # log шкала
    plt.subplot(1, 3, 3)
    x_log = []
    y_log = []
    for k, v in degree_prob.items():
        if k > 0 and v > 0:
            x_log.append(math.log(k))
            y_log.append(math.log(v))

    plt.scatter(x_log, y_log)
    plt.title("Распределение степеней (log - log шкала)")
    plt.xlabel("log(Степень)")
    plt.ylabel("log(Вероятность)")

    plt.tight_layout()
    plt.show()

        
    print(f"\n⏱ Время выполнения A5: {end - start:.6f} секунд")
    print("(A5)----------\n")


    print("(B1)----------")
    stats1 = []
    stats2 = []

    
        
    while True:
        user_input = input(
            "Введите процент рандомных вершин который будет удален (для остановки введите слово 'stop')\n")
        graph_with_removed_vertices = {node: set(neighbors) for node, neighbors in graph.items()}
        if user_input.lower() == 'stop':
            break
        start = perf_counter()
        remove_random_vertices(graph_with_removed_vertices, int(user_input))
        count, max_component_after = weekly_connected_components(graph_with_removed_vertices)
        fraction_after_remove = len(max_component_after) / len(graph_with_removed_vertices)
        print(
            f"доля вершин в наибольшей компоненте\nдо удаления {fraction_of_vertices_largest_week_component}\nпосле  {fraction_after_remove}")
        print("до удаления вершин:", len(graph))
        print("после удаления вершин:", len(graph_with_removed_vertices))
        print("размер наибольшей компоненты до удаления:", len(week_max_component))
        print("размер наибольшей компоненты после удаления:", len(max_component_after))
        end = perf_counter()
        print(f"\n⏱ Время выполнения B1: {end - start:.6f} секунд\n\n")
        stats1.append({
            'percent': int(user_input),
            'fraction_before': fraction_of_vertices_largest_week_component,
            'fraction_after': fraction_after_remove,
            'time': end - start,
        })

    print("(B1)----------\n")
    print("\n(B2)----------")
    while True:
        user_input = input(
            "Введите процент вершин наибольшей степени который будет удален  (для остановки введите слово 'stop')\n")
        graph_with_removed_vertices = {node: set(neighbors) for node, neighbors in graph.items()}
        if user_input.lower() == 'stop':
            break
        
        start = perf_counter()
        remove_top_degree_vertices(graph_with_removed_vertices, int(user_input))

        count, max_component = weekly_connected_components(graph_with_removed_vertices)
        fraction_after_remove = len(max_component) / len(graph_with_removed_vertices)
        print(
            f"доля вершин в наибольшей компоненте\nдо удаления {fraction_of_vertices_largest_week_component}\nпосле  {fraction_after_remove}")
        print("до удаления вершин:", len(graph))
        print("после удаления вершин:", len(graph_with_removed_vertices))
        print("размер наибольшей компоненты:", len(max_component))
        end = perf_counter()
        print(f"\n⏱ Время выполнения B2: {end - start:.6f} секунд\n\n")
        stats2.append({
            'percent': int(user_input),
            'fraction_before': fraction_of_vertices_largest_week_component,
            'fraction_after': fraction_after_remove,
            'time': end - start,
        })

    print("\n(B2)----------")
    plot_comparisons(stats1, stats2)
    return week_max_component

