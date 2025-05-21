"""Реализация пункта 2 и 3(исп.  Landmarks-LCA)"""
import random
from basic import bfs_with_path, bfs, to_undirected
from collections import Counter
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
        a, b = random.sample(graph.keys(), 2)
        path = bfs_with_path(graph, a, b)
        if path:
            paths.append(path)
    coverage = Counter()
    for path in paths:
        for v in path:
              coverage[v]+=1
    return coverage.most_common(k)



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

def landmarks_LCA(): 
    return

def print_distance(graph: dict, directed: bool = True):
    # Преобразуем граф в неориентированный, если он ориентированный
    if directed:
        working_graph = to_undirected(graph)
    else:
        working_graph = {node: set(neighbors) for node, neighbors in graph.items()}
    
    # Выбираем ориентиры (5% вершин, но не менее 5 и не более 50)
    num_landmarks = max(5, min(50, len(working_graph) // 20))
    landmarks = highest_degree_selection(num_landmarks, working_graph)
    
    print("Landmarks-Basic Distance Estimation")
    print("----------------------------------")
    print(f"Selected {len(landmarks)} landmarks using highest degree selection\n")
    
    # Выбираем случайные 5 пар вершин для демонстрации
    nodes = list(working_graph.keys())
    test_pairs = []
    for _ in range(5):
        u, v = random.sample(nodes, 2)
        test_pairs.append((u, v))
    
    # Вычисляем точные расстояния с помощью BFS
    exact_distances = {}
    for u, v in test_pairs:
        exact_distances[u, v] = len(bfs_with_path(working_graph, u, v)) - 1 if bfs_with_path(working_graph, u, v) else -1
    
    # Вычисляем оценки расстояний с помощью Landmarks-Basic
    estimated_distances = {}
    for u, v in test_pairs:
        estimated_distances[u, v] = landmarks_basic(working_graph, landmarks, u, v)
    
    # Выводим результаты
    print("Pair\tExact\tEstimated")
    print("----\t-----\t---------\n")
    for (u, v), exact in exact_distances.items():
        estimated = estimated_distances[u, v]
        print(f"{u}-{v}\t{exact}\t{estimated}")
    
    # Вычисляем среднюю ошибку
    total_error = 0
    valid_pairs = 0
    for (u, v), exact in exact_distances.items():
        if exact != -1 and estimated_distances[u, v] != -1:
            total_error += abs(exact - estimated_distances[u, v])
            valid_pairs += 1
    
    if valid_pairs > 0:
        avg_error = total_error / valid_pairs
        print(f"Average absolute error: {avg_error:.2f}\n")
    else:
        print("No valid pairs for error calculation\n")
    
    # Дополнительная статистика
    reachable_pairs = sum(1 for d in exact_distances.values() if d != -1)
    estimated_reachable = sum(1 for d in estimated_distances.values() if d != -1)
    
    print(f"Reachable pairs (exact): {reachable_pairs}/5")
    print(f"Reachable pairs (estimated): {estimated_reachable}/5")
    print("----------------------------------")