import random
from collections import defaultdict, deque
import time
import os
import glob

def read_graph(file_path):
    graph = defaultdict(set)
    with open(file_path, 'r') as f:
        for line in f:
            u, v = map(int, line.strip().split())
            if u != v:
                graph[u].add(v)
                graph[v].add(u)
    return graph

def bfs_spt(graph, start):
    parent = {}
    distance = {start: 0}
    queue = deque([start])
    parent[start] = None
    while queue:
        u = queue.popleft()
        for v in graph[u]:
            if v not in distance:
                distance[v] = distance[u] + 1
                parent[v] = u
                queue.append(v)
    return distance, parent

def path_to_root(parents, s):
    path = []
    while s is not None:
        path.append(s)
        s = parents.get(s)
    return path

def landmarks_sc(graph, s, t, landmark_parents, landmark_dists):
    best = float('inf')
    for lm in landmark_parents:
        if s not in landmark_dists[lm] or t not in landmark_dists[lm]:
            continue
        path_s = path_to_root(landmark_parents[lm], s)
        path_t = path_to_root(landmark_parents[lm], t)
        set_s, set_t = set(path_s), set(path_t)
        common = set_s & set_t
        if not common:
            continue
        lca = max(common, key=lambda x: landmark_dists[lm][x])
        dist = landmark_dists[lm][s] + landmark_dists[lm][t] - 2 * landmark_dists[lm][lca]
        best = min(best, dist)
        for u in path_s:
            for v in path_t:
                if v in graph[u]:
                    d = (landmark_dists[lm][s] - landmark_dists[lm][u]) + 1 + (landmark_dists[lm][t] - landmark_dists[lm][v])
                    best = min(best, d)
    return best if best != float('inf') else None

def select_landmarks(graph, k, strategy='degree', M=100):
    if strategy == 'random':
        return random.sample(list(graph.keys()), k)
    elif strategy == 'degree':
        return sorted(graph.keys(), key=lambda x: len(graph[x]), reverse=True)[:k]
    elif strategy == 'coverage':
        P = []
        nodes = list(graph.keys())
        for _ in range(M):
            s, t = random.sample(nodes, 2)
            dist, parent = bfs_spt(graph, s)
            if t in parent:
                path = []
                v = t
                while v is not None:
                    path.append(v)
                    v = parent[v]
                P.append(set(path))
        coverage = defaultdict(int)
        for path in P:
            for v in path:
                coverage[v] = coverage[v] + 1
        return sorted(coverage, key=coverage.get, reverse=True)[:k]

def evaluate(graph, landmarks, sample_size=100):
    landmark_dists = {}
    landmark_parents = {}
    for lm in landmarks:
        dists, parents = bfs_spt(graph, lm)
        landmark_dists[lm] = dists
        landmark_parents[lm] = parents
    errors = []
    exact_times = []
    approx_times = []
    nodes = list(graph.keys())
    for _ in range(sample_size):
        s, t = random.sample(nodes, 2)
        t0 = time.perf_counter()
        dist_map = bfs_spt(graph, s)[0]
        d_exact = dist_map.get(t)
        t1 = time.perf_counter()
        t2 = time.perf_counter()
        d_approx = landmarks_sc(graph, s, t, landmark_parents, landmark_dists)
        t3 = time.perf_counter()
        if d_exact is not None and d_approx is not None:
            errors.append(abs(d_exact - d_approx))
            exact_times.append(t1 - t0)
            approx_times.append(t3 - t2)
    return {
        "avg_error": sum(errors) / len(errors),
        "max_error": max(errors),
        "avg_exact_time": sum(exact_times) / len(exact_times),
        "avg_approx_time": sum(approx_times) / len(approx_times),
        "num_pairs": len(errors)
    }

txt_files = sorted(glob.glob(os.path.join("*.txt")))
for i, file in enumerate(txt_files, 1):
    print(f"\n[{i}/{len(txt_files)}] Файл: {os.path.basename(file)}")
    graph = read_graph(os.path.basename(file))
    for strategy in ['random', 'degree', 'coverage']:
        for k in [10, 20, 30, 40, 50]:
            landmarks = select_landmarks(graph, k, strategy=strategy)
            res = evaluate(graph, landmarks, sample_size=100)
            print(f"Strategy: {strategy}, k={k}, Avg error: {res['avg_error']}, Max error: {res['max_error']}, Approx time: {res['avg_approx_time']}s")