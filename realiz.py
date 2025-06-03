import random
import time
from collections import defaultdict, deque
import numpy as np
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

def bfs_distances(graph, source):
    dist = {source: 0}
    queue = deque([source])
    while queue:
        node = queue.popleft()
        for neighbor in graph[node]:
            if neighbor not in dist:
                dist[neighbor] = dist[node] + 1
                queue.append(neighbor)
    return dist

def select_highest_degree(graph, k):
    degree_list = sorted(graph.items(), key=lambda item: len(item[1]), reverse=True)
    return [node for node, _ in degree_list[:k]]

def select_random(graph, k):
    return random.sample(list(graph.keys()), k)

def select_best_coverage(graph, k, M=100):
    P = []
    nodes = list(graph.keys())
    for _ in range(M):
        s, t = random.sample(nodes, 2)
        dist = bfs_distances(graph, s)
        path = []
        if t in dist:
            current = t
            while current != s:
                for neighbor in graph[current]:
                    if dist.get(neighbor, float('inf')) == dist[current] - 1:
                        path.append(current)
                        current = neighbor
                        break
            path.append(s)
            P.append(set(path))
    Vp = set.union(*P) if P else set()
    c = defaultdict(int)
    for v in Vp:
        c[v] = sum(1 for path in P if v in path)
    landmarks = []
    for _ in range(k):
        if not c:
            break
        u = max(c, key=c.get)
        landmarks.append(u)
        c.pop(u)
    return landmarks

def precompute_landmarks(graph, landmark_nodes):
    landmark_dists = {}
    for lm in landmark_nodes:
        landmark_dists[lm] = bfs_distances(graph, lm)
    return landmark_dists

def landmark_distance(s, t, landmark_dists):
    approx = float('inf')
    for lm, dist_map in landmark_dists.items():
        if s in dist_map and t in dist_map:
            approx = min(approx, dist_map[s] + dist_map[t])
    return approx if approx != float('inf') else None

def evaluate(graph, landmark_nodes, landmark_dists, num_pairs=100):
    nodes = list(graph.keys())
    errors = []
    exact_times = []
    approx_times = []
    for _ in range(num_pairs):
        s, t = random.sample(nodes, 2)
        t0 = time.perf_counter()
        d_exact_map = bfs_distances(graph, s)
        d_exact = d_exact_map.get(t, None)
        t1 = time.perf_counter()
        t2 = time.perf_counter()
        d_approx = landmark_distance(s, t, landmark_dists)
        t3 = time.perf_counter()
        if d_exact is not None and d_approx is not None:
            errors.append(abs(d_exact - d_approx))
            exact_times.append(t1 - t0)
            approx_times.append(t3 - t2)
    return {
        'avg_error': np.mean(errors),
        'max_error': np.max(errors),
        'avg_exact_time': np.mean(exact_times),
        'avg_approx_time': np.mean(approx_times)
    }

def print_results(*results):
    print("\nСравнение стратегий выбора ландмарок:\n")
    headers = ["Стратегия", "Средн. ошибка", "Макс. ошибка", "Точное (ср)", "Приближ. (ср)", "Время выбора", "Общее время"]
    row_format = "{:<16} {:<14} {:<12} {:<14} {:<14} {:<14} {:<14}"
    print(row_format.format(*headers))
    print("-" * 100)
    for res in results:
        print(row_format.format(
            res["selection"],
            f"{res['avg_error']}",
            f"{res['max_error']}",
            f"{res['avg_exact_time']}",
            f"{res['avg_approx_time']}",
            f"{res['precompute_time']} секунд",
            f"{res['total_time']} секунд"
        ))

txt_files = sorted(glob.glob(os.path.join("*.txt")))
for i, file in enumerate(txt_files, 1):
    print(f"\n[{i}/{len(txt_files)}] Файл: {os.path.basename(file)}")
    landmark_count = [10, 20, 30, 40, 50]
    graph = read_graph(os.path.basename(file))
    for k in landmark_count:
        print(f"\n Количество ландмарок: {k}")
        start_time = time.perf_counter()
        landmarks_high = select_highest_degree(graph, k)
        pre_time = time.perf_counter()
        dists_high = precompute_landmarks(graph, landmarks_high)
        eval_high = evaluate(graph, landmarks_high, dists_high, num_pairs=100)
        end_time = time.perf_counter()
        eval_high["selection"] = "highest_degree"
        eval_high["total_time"] = end_time - start_time
        eval_high["precompute_time"] = pre_time - start_time
        start_time = time.perf_counter()
        landmarks_rand = select_random(graph, k)
        pre_time = time.perf_counter()
        dists_rand = precompute_landmarks(graph, landmarks_rand)
        eval_rand = evaluate(graph, landmarks_rand, dists_rand, num_pairs=100)
        end_time = time.perf_counter()
        eval_rand["selection"] = "random"
        eval_rand["total_time"] = end_time - start_time
        eval_rand["precompute_time"] = pre_time - start_time
        start_time = time.perf_counter()
        landmarks_best = select_best_coverage(graph, k, M=100)
        pre_time = time.perf_counter()
        dists_best = precompute_landmarks(graph, landmarks_best)
        eval_best = evaluate(graph, landmarks_best, dists_best, num_pairs=100)
        end_time = time.perf_counter()
        eval_best["selection"] = "best_coverage"
        eval_best["total_time"] = end_time - start_time
        eval_best["precompute_time"] = pre_time - start_time
        print_results(eval_high, eval_rand, eval_best)