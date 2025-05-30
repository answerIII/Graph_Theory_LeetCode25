from collections import deque
import math
import time
from functools import wraps
import multiprocessing
import random
import pandas as pd

def load_graph(file, directed=False):
    edges = []
    nodes = set()
    adjacency = {}

    with open(file, 'r') as file:
        for line in file:
            # skip comments and empty lines
            if line.startswith('#') or line.startswith('%') or not line.strip():
                continue
            u, v = map(int, line.strip().split())

            edges.append((u, v))
            nodes.update([u, v]) # add both nodes to the set

            if u not in adjacency:
                adjacency[u] = set()
            adjacency[u].add(v)

            if not directed:
                if v not in adjacency:
                    adjacency[v] = set()
                adjacency[v].add(u)

    return edges, nodes, adjacency

# TIME MEASUREMENT
recorder = {
    'times': {},
    'mae': {},
}
def timeit(step_name, recorder=recorder):
    def decorator(f):
        @wraps(f)
        def wrapper(*args, **kwargs):
            t0 = time.perf_counter()
            result = f(*args, **kwargs)
            elapsed = time.perf_counter() - t0
            recorder['times'].setdefault(step_name, []).append(elapsed)
            return result
        return wrapper
    return decorator

# SELECT LANDMARKS
@timeit('select_landmarks')
def select_landmarks_randomly(nodes, adjacency, k):
    len_nodes = len(nodes)
    if k > len_nodes:
        raise ValueError(f"you provided k={k} landmarks, but the graph has only {len_nodes} nodes")
    return random.sample(list(nodes), k)
@timeit('select_landmarks')
def select_landmarks_by_highest_degree(nodes, adjacency, k):
    if k > len(nodes):
        raise ValueError(f"you provided k={k}, but the graph has only {len(nodes)} nodes")
    
    # list of (node, degree)
    degs = [(v, len(adjacency.get(v, []))) for v in nodes]
    # sort by degree descending
    degs.sort(key=lambda x: x[1], reverse=True)
    # take the top k nodes
    return [v for v, d in degs[:k]]
def shortest_path(adjacency, s, t):
    if s == t:
        return [s]
    pred = {s: None}
    queue = deque([s])
    while queue:
        u = queue.popleft()
        for nbr in adjacency.get(u, ()):
            if nbr not in pred:
                pred[nbr] = u
                if nbr == t:
                    # reconstruct path
                    path = [t]
                    while path[-1] != s:
                        path.append(pred[path[-1]])
                    return list(reversed(path))
                queue.append(nbr)
    return []  # no path
@timeit('select_landmarks')
def select_landmarks_by_best_coverage(nodes, adjacency, k):
    M = 100  # number of sample paths to build coverage set
    nodes_list = list(nodes)

    # 8–13: sample M paths
    P = []
    for _ in range(M):
        s, t = random.sample(nodes_list, 2)
        p = shortest_path(adjacency, s, t)
        if p:
            P.append(p)

    # 14: VP = all vertices appearing in any path
    VP = set(v for path in P for v in path)

    landmarks = []
    remaining_paths = P.copy()

    # 15–19: greedy coverage
    for _ in range(k):
        # count coverage of each v in VP
        coverage = {v: 0 for v in VP}
        for path in remaining_paths:
            for v in path:
                coverage[v] += 1

        # pick v with max coverage
        u = max(coverage, key=lambda v: coverage[v])
        landmarks.append(u)

        # remove all paths covered by u
        remaining_paths = [path for path in remaining_paths if u not in path]

        # if no paths left, break early
        if not remaining_paths:
            break

    return landmarks

# LANDMARKS_BASIC
def run_bfs_n_return_distances(adjacency, source):
    distances = {source: 0}
    queue = deque([source])

    while queue: # is not empty
        u = queue.popleft()
        adjacency_u = adjacency.get(u, [])
        for v in adjacency_u:
            if v not in distances:
                distances[v] = distances[u] + 1
                queue.append(v)
    return distances
@timeit('compute_landmark_distances')
def compute_landmark_distances(adjacency, landmarks):
    landmark_distances = {}
    for cur_landmark in landmarks:
        landmark_distances[cur_landmark] = run_bfs_n_return_distances(adjacency, source=cur_landmark)
    return landmark_distances
@timeit('estimate_distance_basic')
def estimate_distance_basic(s, t, landmark_distances):
    all_estimates = []
    for l, ld_l in landmark_distances.items():
        ds = ld_l.get(s, float('inf'))
        dt = ld_l.get(t, float('inf'))
        all_estimates.append(ds + dt)
    ans = min(all_estimates) if all_estimates else float('inf')
    return ans
@timeit('landmarks_basic')
def landmarks_basic(adjacency, nodes, s, t, k, select_landmarks_alg):
    landmarks = select_landmarks_alg(nodes, adjacency, k)
    landmark_distances = compute_landmark_distances(adjacency, landmarks)
    distance = estimate_distance_basic(s, t, landmark_distances)
    return distance

# LANDMARKS_SC
@timeit('build_spt')
def build_spt(adjacency, landmark):
    spt = {landmark: None}
    visited = {landmark}
    queue = deque([landmark])

    while queue:
        curr = queue.popleft()
        for nbr in adjacency.get(curr, ()):
            if nbr not in visited:
                visited.add(nbr)
                spt[nbr] = curr
                queue.append(nbr)

    return spt
@timeit('get_path')
def get_path(s, target_path, spt):
    target_path_set = set(target_path) # so membership test is O(1)

    path = [s]
    while s not in target_path_set:
        s = spt[s]
        path.append(s)
    return path
@timeit('distance_sc')
def distance_sc(s, t, adjacency, spt_u, u):
    # π1: path from s up to landmark u
    pi1 = get_path(s, {u}, spt_u)        # [s, …, u]

    # π2: path from t up to any node in π1; ends at LCA
    pi2 = get_path(t, pi1, spt_u)        # [t, …, LCA]
    lca = pi2[-1]

    # π3: path from s up to that LCA
    pi3 = get_path(s, {lca}, spt_u)      # [s, …, LCA]

    # base cost: go t --> LCA and s --> LCA without shortcut
    best = (len(pi2) - 1) + (len(pi3) - 1)

    # try every edge between π2 and π3 to see if it gives a shorter path
    for i, w in enumerate(pi2):
        for j, w_prime in enumerate(pi3):
            if w_prime in adjacency.get(w, ()):
                curr = i + 1 + j
                if curr < best:
                    best = curr

    return best
@timeit('landmarks_sc')
def landmarks_sc(adjacency, nodes, s, t, k, select_landmarks_alg):
    best = float('inf')

    landmarks = select_landmarks_alg(nodes, adjacency, k)
    for u in landmarks:
        spt_u = build_spt(adjacency, u)

        # if either s or t isn't reachable from u, skip this landmark
        if s not in spt_u or t not in spt_u:
            continue

        # otherwise compute the SC‐distance
        est = distance_sc(s, t, adjacency, spt_u, u)
        best = min(best, est)

    return best

def evaluate(graph_path, estimator_fn, methods, k_values, pairs):
    edges, nodes, adjacency = load_graph(graph_path, directed=False)
    rows = []
    for name, select_fn in methods.items():
        print(f"evaluating {name} landmarks selection method")
        for k in k_values:
            print(f"  k={k}")
            recorder['times'].clear()
            errors = []
            counter = 0
            for s, t in pairs:
                true_d = run_bfs_n_return_distances(adjacency, s).get(t, float('inf'))
                est_d = estimator_fn(adjacency, nodes, s, t, k, select_fn)
                if math.isfinite(true_d) and math.isfinite(est_d):
                    errors.append(abs(true_d - est_d))
                counter += 1
                print(f"\t{counter}/{len(pairs)} pairs: true_d={true_d}, est_d={est_d}, error={errors[-1] if errors else 'N/A'}")
            
            min_err = min(errors) if errors else float('nan')
            max_err = max(errors) if errors else float('nan')
            mae = sum(errors)/len(errors) if errors else float('nan')
            total = sum(sum(lst) for lst in recorder['times'].values())
            row = {
                'method': name,
                'k': k,
                'min_error': min_err,
                'max_error': max_err,
                'mae': mae,
                'total_time': total,
            }
            for step, lst in recorder['times'].items():
                row[f'time_{step}'] = sum(lst)
            rows.append(row)
    return pd.DataFrame(rows)

def main():
    # graph_path = "datasets/undirected/CA-AstroPh.txt"
    # graph_path = "datasets/undirected/CA-GrQc.txt"
    # graph_path = "datasets/directed/Wiki-Vote.txt"

    # graph_path = "datasets/undirected/CA-AstroPh.txt"
    # graph_path = "datasets/undirected/CA-GrQc.txt"
    # graph_path = "datasets/directed/Wiki-Vote.txt"
    
    # graph_path = "datasets/undirected/CA-AstroPh.txt"
    # graph_path = "datasets/undirected/CA-GrQc.txt"
    # graph_path = "datasets/directed/Wiki-Vote.txt"
    # graph_path = "datasets/undirected/Email-EuAll.txt"

    # graph_path = "datasets/undirected/CA-AstroPh.txt"
    # graph_path = "datasets/undirected/CA-GrQc.txt"
    # graph_path = "datasets/directed/Wiki-Vote.txt"
    # graph_path = "datasets/undirected/Email-EuAll.txt"

    # graph_path = "datasets/tests/graph_0.txt"
    # graph_path = "datasets/tests/graph_1.txt"
    # graph_path = "datasets/tests/graph_2.txt"
    # graph_path = "datasets/tests/graph_3.txt"
    # graph_path = "datasets/tests/graph_4.txt"
    # graph_path = "datasets/tests/graph_5.txt"
    # graph_path = "datasets/tests/graph_6.txt"
    # graph_path = "datasets/tests/graph_7.txt"
    graph_path = "datasets/tests/graph_8.txt"

    edges, nodes, adjacency = load_graph(graph_path, directed=False)
    print(f"loaded graph with {len(nodes):,} nodes and {len(edges):,} edges")

    num_pairs = 50
    pairs = [tuple(random.sample(list(nodes), 2)) for _ in range(num_pairs)]

    landmark_methods = {
        'random': select_landmarks_randomly,
        'degree': select_landmarks_by_highest_degree,
        'coverage': select_landmarks_by_best_coverage,
    }
    k_values = [5, 10, 20, 50, 100, 200]

    # landmark_methods = {
    #     'degree': select_landmarks_by_highest_degree,
    # }
    num_pairs = 100
    pairs = [tuple(random.sample(list(nodes), 2)) for _ in range(num_pairs)]
    k_values = [100]
    k_values = [5, 10, 20, 50]

    # estimator = landmarks_basic
    estimator = landmarks_sc

    df = evaluate(graph_path, estimator, landmark_methods, k_values, pairs)
    print("\nperformance and accuracy VS select_landmarks_alg(), k")
    print(df)
    results_folder = "results/landmarks_selection_and_k"
    results_folder = "results/accuracy/SC"
    results_folder = "results/accuracy/BASIC"
    results_folder = "results/accuracy/tests"
    # df.to_csv(f"{results_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_BASIC_1.csv", index=False)
    # df.to_csv(f"{results_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_BASIC_2.csv", index=False)
    # df.to_csv(f"{results_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_BASIC_3.csv", index=False)

    # df.to_csv(f"{results_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_SC_1.csv", index=False)
    # df.to_csv(f"{results_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_SC_2.csv", index=False)
    # df.to_csv(f"{results_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_SC_3.csv", index=False)

    # df.to_csv(f"{results_folder}/CA-AstroPh_SC_k100.csv", index=False)
    # df.to_csv(f"{results_folder}/CA-GrQc_SC_k100.csv", index=False)
    # df.to_csv(f"{results_folder}/Wiki-Vote_SC_k100.csv", index=False)
    # df.to_csv(f"{results_folder}/Email-EuAll_SC_k100.csv", index=False)

    # df.to_csv(f"{results_folder}/graph_0_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_0_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_1_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_1_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_2_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_2_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_3_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_3_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_4_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_4_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_5_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_5_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_6_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_6_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_7_BASIC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_7_SC.csv", index=False)
    # df.to_csv(f"{results_folder}/graph_8_BASIC.csv", index=False)
    df.to_csv(f"{results_folder}/graph_8_SC.csv", index=False)

    print("saved results saved to .csv")

if __name__ == "__main__":
    main()