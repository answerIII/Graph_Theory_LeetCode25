from A1 import *
import numpy as np
import random
from basic import *
from tqdm import tqdm

def double_sweep(graph: dict[int, set[int]], component: set[int]) -> int:
    if not component:
        return 0
    start = random.choice(list(component))
    d1 = bfs(graph, start, component)
    farthest = max(d1, key=d1.get)
    d2 = bfs(graph, farthest, component)
    return max(d2.values())

def random_pairwise_distances(graph: dict[int, set[int]], component: set[int], sample_size=500) -> float:
    nodes = list(component)
    if len(nodes) < 2:
        return 0
    sample = random.sample(nodes, min(sample_size, len(nodes)))
    distances = []
    for i in tqdm(range(len(sample)), desc="Processing pairs - random"):
        dists = bfs(graph, sample[i], component)
        for j in range(i + 1, len(sample)):
            if sample[j] in dists:
                distances.append(dists[sample[j]])
    return np.percentile(distances, 90) if distances else 0

def snowball_sampling(graph: dict[int, set[int]], component: set[int], sample_size=500, seed_size=3) -> float:
    if not component:
        return 0
    seed = random.sample(list(component), min(seed_size, len(component)))
    visited = set(seed)
    frontier = set(seed)
    while len(visited) < sample_size and frontier:
        next_frontier = set()
        for node in frontier:
            for neighbor in graph.get(node, set()):
                if neighbor in component and neighbor not in visited:
                    next_frontier.add(neighbor)
        visited.update(next_frontier)
        frontier = next_frontier
    nodes = list(visited)
    distances = []
    for i in tqdm(range(len(nodes)), desc="Processing pairs - snowball"):
        dists = bfs(graph, nodes[i], visited)
        for j in range(i + 1, len(nodes)):
            if nodes[j] in dists:
                distances.append(dists[nodes[j]])
    return np.percentile(distances, 90) if distances else 0