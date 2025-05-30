import networkx as nx
import random
from collections import Counter
def select_landmarks_random(G, k):
    return random.sample(list(G.nodes()), k)

def select_landmarks_best_coverage(G, k, M):
    P = []
    nodes = list(G.nodes())
    while len(P) < M:
        s, t = random.sample(nodes, 2)
        try:
            path = nx.shortest_path(G, s, t)
            P.append(path)
        except nx.NetworkXNoPath:
            continue
    all_nodes_in_paths = [v for path in P for v in path]
    counter = Counter(all_nodes_in_paths)
    selected_landmarks = []
    used_nodes = set()
    for _ in range(k):
        best_node = None
        best_count = -1
        for node, count in counter.items():
            if node not in used_nodes and count > best_count:
                best_node = node
                best_count = count
        if best_node is None:
            break
        selected_landmarks.append(best_node)
        used_nodes.add(best_node)
    return selected_landmarks

def select_landmarks_highest_degree(G, k):
    degrees = dict(G.degree())
    sorted_nodes = sorted(degrees, key=degrees.get, reverse=True)
    return sorted_nodes[:k]

def precompute_distances(G, landmarks):
    dist_from_landmarks = {}
    for u in landmarks:
        dist_from_landmarks[u] = nx.single_source_shortest_path_length(G, u)
    return dist_from_landmarks

def landmarks_basic_estimate(s, t, landmark_distances):
    estimates = []
    for l in landmark_distances:
        d_l = landmark_distances[l]
        if s in d_l and t in d_l:
            estimates.append(d_l[s] + d_l[t])
    return min(estimates)

def run_landmarks_basic(G, k, sample_pairs):
    landmarks = select_landmarks_highest_degree(G, k)
    print(f"Выбранные ландмарки (высшая степень): {landmarks}")
    landmark_distances = precompute_distances(G, landmarks)
    nodes = list(G.nodes())
    for _ in range(sample_pairs):
        s, t = random.sample(nodes, 2)
        try:
            true_distance = nx.shortest_path_length(G, s, t)
        except nx.NetworkXNoPath:
            continue
        est_distance = landmarks_basic_estimate(s, t, landmark_distances)
        print(f"{s}–{t} | Истинное расстояние: {true_distance}, оценка (высшая степень): {est_distance}")
    
    landmarks = select_landmarks_best_coverage(G, k, sample_pairs)
    print(f"Выбранные ландмарки (наилучшее покрытие): {landmarks}")
    landmark_distances = precompute_distances(G, landmarks)
    nodes = list(G.nodes())
    for _ in range(sample_pairs):
        s, t = random.sample(nodes, 2)
        try:
            true_distance = nx.shortest_path_length(G, s, t)
        except nx.NetworkXNoPath:
            continue
        est_distance = landmarks_basic_estimate(s, t, landmark_distances)
        print(f"{s}–{t} | Истинное расстояние: {true_distance}, оценка (наилучшее покрытие): {est_distance}")

    landmarks = select_landmarks_random(G, k)
    print(f"Выбранные ландмарки (случайно): {landmarks}")
    landmark_distances = precompute_distances(G, landmarks)
    nodes = list(G.nodes())
    for _ in range(sample_pairs):
        s, t = random.sample(nodes, 2)
        try:
            true_distance = nx.shortest_path_length(G, s, t)
        except nx.NetworkXNoPath:
            continue
        est_distance = landmarks_basic_estimate(s, t, landmark_distances)
        print(f"{s}–{t} | Истинное расстояние: {true_distance}, оценка (случайно): {est_distance}")

file_path = 'Email-EuAll.txt'
G = nx.Graph()
with open(file_path, 'r') as f:
    for line in f:
        u, v = map(int, line.strip().split())
        G.add_edge(u, v)
run_landmarks_basic(G, k=10, sample_pairs=10)