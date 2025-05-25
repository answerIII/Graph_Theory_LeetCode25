import random
from A1 import *
from tqdm import tqdm
def remove_random_vertices(graph: dict,percent: int):
    vertices_to_remove = random.sample(list(graph.keys()), len(graph)*percent//100)
    for v in tqdm(vertices_to_remove, desc="Removing random vertices"):
        graph.pop(v)
    for list_of_neighbors in tqdm(graph.values(), desc="Cleaning neighbor lists"):
        list_of_neighbors.difference_update(vertices_to_remove)
            
def remove_top_degree_vertices(graph: dict, percent: int):
    degrees = dict()
    for l in graph:
        degrees[l] = len(graph[l])
    vertices_to_remove = list(degrees.keys())
    vertices_to_remove.sort(key=degrees.get, reverse=True)
    vertices_to_remove = vertices_to_remove[:len(graph)*percent//100]
    for v in tqdm(vertices_to_remove, desc="Removing top-degree vertices"):
        graph.pop(v)
    for list_of_neighbors in tqdm(graph.values(), desc="Cleaning neighbor lists"):
        list_of_neighbors.difference_update(vertices_to_remove)