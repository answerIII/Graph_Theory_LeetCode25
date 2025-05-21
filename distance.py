"""Реализация пункта 2 и 3(исп.  Landmarks-LCA)"""
import random
from basic import bfs_with_path
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



def landmarks_basic(): # соня, это ты делаешь
    return

def landmarks_LCA(): 
    return