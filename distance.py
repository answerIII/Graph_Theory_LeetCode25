"""Реализация пункта 2 и 3(исп.  Landmarks-LCA)"""
import random
from basic import bfs_with_path, bfs, to_undirected, bfs_with_path_and_distance
from collections import Counter
from time import perf_counter

"""-----функции для выбора ориентиров-----"""
#  на вход передаем количество используемых ориентиров
# возвращаем выбранные ориентиры
def random_selection(k: int, graph: dict):
    """рандомный выбор"""
    return random.sample(list(graph.keys()), k)

def highest_degree_selection(k: int, graph: dict):
    """выбираем вершины с наибольшей степенью"""
    degrees = {v: len(neighbors) for v, neighbors in graph.items()}
    return sorted(degrees, key=degrees.get, reverse=True)[:k]

def best_coverage_selection(k: int, graph: dict):
    """берем случайные пары, выбираем вершины, которые чаще всего встречаются в путях"""
    paths = []
    for _ in range(k):
        a, b = random.sample(list(graph.keys()), 2)
        path = bfs_with_path(graph, a, b)
        if path:
            paths.append(path)
    coverage = Counter()
    for path in paths:
        for v in path:
              coverage[v]+=1
    return [v for v, _ in coverage.most_common(k)]



def landmarks_basic(graph: dict, landmarks: list, u: int, v: int) -> int:
    min_distance = float('inf')
    
    # Предварительно вычисляем расстояния от всех ориентиров ко всем вершинам
    landmark_distances = {}
    for landmark in landmarks:
        landmark_distances[landmark] = bfs(graph, landmark)
    
    # Для каждой пары ориентиров вычисляем оценку расстояния
    for landmark in landmarks:
        if u in landmark_distances[landmark] and v in landmark_distances[landmark]:
            distance_estimate = abs(landmark_distances[landmark][u] - landmark_distances[landmark][v])
            if distance_estimate < min_distance:
                min_distance = distance_estimate
                
    return min_distance if min_distance != float('inf') else -1


def print_distance(graph: dict, landmark_selection_option = 2):
    # Преобразуем граф в неориентированный, если он ориентированный
    """if directed:
        working_graph = to_undirected(graph)
    else:
        working_graph = {node: set(neighbors) for node, neighbors in graph.items()}"""
        
    working_graph = graph
    start = perf_counter()
    # Выбираем ориентиры (5% вершин, но не менее 5 и не более 50)
    num_landmarks = max(5, min(50, len(working_graph) // 20))
    if landmark_selection_option == 1:
        landmarks = random_selection(num_landmarks, working_graph)
    if landmark_selection_option == 2:
        landmarks = highest_degree_selection(num_landmarks, working_graph)
    if landmark_selection_option == 3:
        landmarks = best_coverage_selection(num_landmarks, working_graph)
    end = perf_counter()
    print(f"\n⏱ Время выбора ориентиров: {end - start:.6f} секунд")
   

    print("Landmarks-Basic")
    print(f"Использовано {len(landmarks)} ориентиров\n")
    
    # Выбираем случайные 5 пар вершин для демонстрации
    nodes = list(working_graph.keys())
    test_pairs = []
    for _ in range(5):
        u, v = random.sample(nodes, 2)
        test_pairs.append((u, v))
    
    # Вычисляем точные расстояния с помощью BFS
    exact_distances = {}
    for u, v in test_pairs:
        path = bfs_with_path(working_graph, u, v)
        exact_distances[u, v] = len(path) - 1 if path else -1
    
    # Вычисляем оценки расстояний с помощью Landmarks-Basic
    start = perf_counter()
    estimated_distances = {}
    for u, v in test_pairs:
        estimated_distances[u, v] = landmarks_basic(working_graph, landmarks, u, v)
    end = perf_counter()
    # Выводим результаты
    print(f"{'Пара':<20}{'Реальное':<20}{'Оценка':<20}")
    print("-" * 45)   
    for (u, v), exact in exact_distances.items():
        estimated = estimated_distances[u, v]
        print(f"{f'{u}, {v}':<20}{str(exact):<20}{str(estimated):<20}")
    
    # Вычисляем среднюю ошибку
    total_error = 0
    valid_pairs = 0
    for (u, v), exact in exact_distances.items():
        if exact != -1 and estimated_distances[u, v] != -1:
            total_error += abs(exact - estimated_distances[u, v])
            valid_pairs += 1
    
    if valid_pairs > 0:
        avg_error = total_error / valid_pairs
        print(f"\nСредняя абсолютная ошибка (Basic): {avg_error:.2f}")
    else:
        print("No valid pairs for error calculation\n")
    print(f"\n⏱ Время выполнения Landmarks-Basic: {end - start:.6f} секунд\n\n")

    # Дополнительная статистика
    # reachable_pairs = sum(1 for d in exact_distances.values() if d != -1)
    # estimated_reachable = sum(1 for d in estimated_distances.values() if d != -1)
    
    # print(f"Reachable pairs (exact): {reachable_pairs}/5")
    # print(f"Reachable pairs (estimated): {estimated_reachable}/5")
    # print("----------------------------------")
    
    print("\n\n\nLandmarks-LCA")
    data = landmarks_LCA(graph, landmarks)
    lca_distances = {}
    for a,b in test_pairs:
        min_dist = float('inf')
        for l in landmarks:
            parents = data[l]["parents"]
            distances = data[l]["distances"]
            
            if a not in parents or b not in parents:
                continue
            path_a = build_path(parents, a)
            path_b = build_path(parents, b)
            
            lca = find_lca(path_a, path_b)  #  первая общая вершина
            if lca is None:
                continue
            
            dist_a_landmark = distances[a]
            dist_b_landmark = distances[b]
            dist_lca_landmark = distances[lca]
            dist_a_lca = dist_a_landmark - dist_lca_landmark
            dist_b_lca = dist_b_landmark - dist_lca_landmark
            min_dist = min(dist_a_lca+dist_b_lca, min_dist)
        if min_dist != float('inf'):
            lca_distances[a, b] = min_dist
        else:
             lca_distances[a, b] = -1
    end = perf_counter()
    print(f"{'Пара':<20}{'Реальное':<20}{'Оценка':<20}")
    print("-" * 45)
    for (a, b), exact in exact_distances.items():
        estimate =  lca_distances[a, b]
        print(f"{f'{a}, {b}':<20}{str(exact):<20}{str(estimate):<20}")
    
    # Средняя ошибка
    total_error = 0
    valid_pairs = 0
    for (a, b), exact in exact_distances.items():
        est = lca_distances[a, b]
        if exact != -1 and est != -1:
            total_error += abs(exact - est)
            valid_pairs += 1
    if valid_pairs > 0:
        avg_error = total_error / valid_pairs
        print(f"\nСредняя абсолютная ошибка (LCA): {avg_error:.2f}")
    else:
        print("No valid pairs for LCA error calculation\n")
    print(f"\n⏱ Время выполнения Landmarks-LCA: {end - start:.6f} секунд")

    
    
    
def landmarks_LCA(graph:dict, landmarks: list, ): 
    data = dict() #  дерево кратч путей и расстояния 
    for l in landmarks:
        parents, distances = bfs_with_path_and_distance(graph, l)
        data[l] = { "parents": parents, "distances": distances}
    return data

def build_path(parents, vertice):
    path = []
    cur_node = vertice
    while cur_node is not None:
        path.append(cur_node)
        cur_node = parents[cur_node]
    return path
    
def find_lca(path_a: list, path_b: list):
    path_a2 = set(path_a)
    for v in path_b:
        if v in path_a2:
            return v
    return None
