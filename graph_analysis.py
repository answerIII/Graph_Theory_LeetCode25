import csv
import os
import random

results_log = {}

def load_graph(file, directed=False):
    edges = []
    nodes = set()
    adjacency = {}

    with open(file, 'r') as file:
        for line in file:
            if line.startswith('#') or not line.strip():
                continue
            u, v = map(int, line.strip().split())

            edges.append((u, v))
            nodes.update([u, v])

            if u not in adjacency:
                adjacency[u] = set()
            adjacency[u].add(v)

            if not directed:
                if v not in adjacency:
                    adjacency[v] = set()
                adjacency[v].add(u)

    return edges, nodes, adjacency

def dfs(adjacency, node, visited, component=None, post_order=None):
    stack = [(node, False)]
    while stack:
        curr_node, children_visited = stack.pop()
        if curr_node not in visited:
            if children_visited:
                if post_order is not None:
                    post_order.append(curr_node)
            else:
                visited.add(curr_node)
                if component is not None:
                    component.append(curr_node)
                stack.append((curr_node, True))
                for neighbor in adjacency.get(curr_node, []):
                    if neighbor not in visited:
                        stack.append((neighbor, False))

def bfs(adjacency, start, allowed_nodes=None):
    dist = {start: 0}
    queue = [start]
    while queue:
        node = queue.pop(0)
        for neighbor in adjacency.get(node, []):
            if allowed_nodes is not None and neighbor not in allowed_nodes:
                continue
            if neighbor not in dist:
                dist[neighbor] = dist[node] + 1
                queue.append(neighbor)
    return dist

                        
def reverse_graph(edges):
    reversed_graph = {}
    for u, v in edges:
        if v not in reversed_graph:
            reversed_graph[v] = set()
        reversed_graph[v].add(u)
    return reversed_graph

def count_links(adjacency: dict, node: int):
    neighbors = adjacency.get(node, set())
    links = 0
    neighbor_list = list(neighbors)
    n = len(neighbor_list)
    for i in range(n):
        u = neighbor_list[i]
        for j in range(i + 1, n):
            v = neighbor_list[j]
            if u in adjacency and v in adjacency[u]:
                links += 1
    return links

def count_triples(adjacency: dict):
    triples = 0.0
    for neighbors in adjacency.values():
        k = len(neighbors)
        triples += k * (k - 1) / 2
    return triples

def compute_local_metrics(adjacency: dict):
    clustering_coeffs = []
    triangles_counted = 0

    for node, neighbors in adjacency.items():
        k = len(neighbors)    
        if k < 2:
            clustering_coeffs.append(0.0)
            continue

        links = count_links(adjacency, node)
        Ci = (2 * links) / (k * (k - 1))
        clustering_coeffs.append(Ci)

        triangles_counted += links

    total_triangles = triangles_counted // 3
    average_clustering = (
        sum(clustering_coeffs) / len(clustering_coeffs)
        if clustering_coeffs else 0.0
    )
    return total_triangles, average_clustering

def get_largest_weak_component_size_absolute(current_nodes, current_adj, original_size):
    visited = set()
    max_size = 0
    for node in current_nodes:
        if node not in visited:
            component = []
            dfs(current_adj, node, visited, component)
            max_size = max(max_size, len(component))
    return max_size / original_size if original_size > 0 else 0

def remove_nodes(nodes_to_remove, adj):
    new_nodes = set(adj.keys()) - set(nodes_to_remove)
    new_adj = {}
    for u in new_nodes:
        neighbors = adj[u]
        new_adj[u] = set(v for v in neighbors if v in new_nodes)
    return list(new_nodes), new_adj

def get_percentile(data, percent):
    if not data:
        return None
    data = sorted(data)
    n = len(data)
    k = (percent / 100) * (n - 1)
    i = int(k)
    g = k - i
    if i + 1 < n:
        return data[i] + g * (data[i + 1] - data[i])
    else:
        return data[i]

def log_result(metric_name, value, graph_name="graph"):
    if metric_name not in results_log:
        results_log[metric_name] = {}
    results_log[metric_name][graph_name] = value

def save_results_to_csv(filename="graph_metrics_results.csv"):
    all_graph_names = set()
    for values in results_log.values():
        all_graph_names.update(values.keys())
    all_graph_names = sorted(all_graph_names)

    with open(filename, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        header = ["Metric"] + all_graph_names
        writer.writerow(header)

        for metric, value_dict in results_log.items():
            row = [metric]
            for graph in all_graph_names:
                row.append(value_dict.get(graph, ""))
            writer.writerow(row)

def graph_analysis_1(file, directed=False):
    edges, nodes, adjacency = load_graph(file, directed)
    num_nodes = len(nodes)
    num_edges = len(edges)
    max_edges = num_nodes * (num_nodes - 1)
    if not directed:
        max_edges //= 2
    density = num_edges / max_edges if max_edges else 0
    visited = set()
    weak_components = []
    for node in nodes:
        if node not in visited:
            component = []
            dfs(adjacency, node, visited, component)
            weak_components.append(component)
    num_weak_components = len(weak_components)
    share_max_weak_component = max(len(c) for c in weak_components) / num_nodes if num_nodes else 0

    log_result("Number of nodes", num_nodes, file)
    log_result("Number of edges", num_edges, file)
    log_result("Graph density", round(density, 5), file)
    log_result("Number of weakly connected components", num_weak_components, file)
    log_result("Share of nodes in largest weak component", round(share_max_weak_component, 5), file)

    if directed:
        reversed_graph = reverse_graph(edges)
        visited = set()
        post_order = []
        for node in nodes:
            if node not in visited:
                dfs(reversed_graph, node, visited, post_order)
        visited = set()
        strong_components = []
        for node in reversed(post_order):
            if node not in visited:
                component = []
                dfs(adjacency, node, visited, component)
                strong_components.append(component)
        num_strong_components = len(strong_components)
        share_max_strong_component = max(len(c) for c in strong_components) / num_nodes if num_nodes else 0

        log_result("Number of strongly connected components", num_strong_components, file)
        log_result("Share of nodes in largest strong component", round(share_max_strong_component, 5), file)

def graph_analysis_2(file, sample_size=500):
    _, nodes, adjacency = load_graph(file)
    visited = set()
    components = []
    for node in nodes:
        if node not in visited:
            component = []
            dfs(adjacency, node, visited, component=component)
            components.append(component)
    if not components:
        return
    largest_component = max(components, key=len)
    largest_component_set = set(largest_component)
    tmp_adjacency = {
        u: set(v for v in adjacency.get(u, set()) if v in largest_component_set)
        for u in largest_component
    }
    # A
    r = random.choice(largest_component)
    dist_r = bfs(tmp_adjacency, r)
    a = max(dist_r, key=dist_r.get)
    dist_a = bfs(tmp_adjacency, a)
    b = max(dist_a, key=dist_a.get)
    diameter = dist_a[b]
    log_result("Diameter via double BFS", diameter, file)
    # B
    sample_nodes = random.sample(largest_component, min(sample_size, len(largest_component)))
    dist = []
    for u in sample_nodes:
        dist_u = bfs(tmp_adjacency, u, allowed_nodes=set(sample_nodes))
        for v in sample_nodes:
            if v != u and v in dist_u:
                dist.append(dist_u[v])
    if dist:
        log_result("Estimated diameter from sample", max(dist), file)
        log_result("90th percentile distance (sample)", round(get_percentile(dist, 90), 5), file)
    # C
    start_node_1, start_node_2 = None, None
    for u in largest_component:
        for v in tmp_adjacency.get(u, []):
            if u < v:
                start_node_1, start_node_2 = u, v
                break
        if start_node_1 is not None:
            break
    if start_node_1 is None:
        return
    dist = {}
    queue = []
    snowball = set()
    for s in [start_node_1, start_node_2]:
        dist[s] = 0
        queue.append(s)
        snowball.add(s)
    while queue and len(snowball) < sample_size:
        node = queue.pop(0)
        for neighbor in tmp_adjacency.get(node, []):
            if neighbor not in dist:
                dist[neighbor] = dist[node] + 1
                snowball.add(neighbor)
                queue.append(neighbor)
                if len(snowball) >= sample_size:
                    break
    if dist:
        log_result("Estimated diameter from snowball", max(dist.values()), file)
        log_result("90th percentile distance (snowball)", round(get_percentile(list(dist.values()), 90), 5), file)

def graph_analysis_3(file):
    _, _, adjacency = load_graph(file)
    total_triangles, avg_clustering = compute_local_metrics(adjacency)
    triples = count_triples(adjacency)
    global_clustering = 3 * total_triangles / triples if triples else 0.0
    log_result("Number of triangles", total_triangles, file)
    log_result("Average local clustering coefficient", round(avg_clustering, 5), file)
    log_result("Global clustering coefficient", round(global_clustering, 5), file)

def graph_analysis_4(file):
    _, nodes, adjacency = load_graph(file)
    
    visited = set()
    weak_components = []
    for node in nodes:
        if node not in visited:
            component = []
            dfs(adjacency, node, visited, component)
            weak_components.append(component)

    if not weak_components:
        return

    largest_component = max(weak_components, key=len)
    largest_component_set = set(largest_component)
    
    tmp_adjacency = {}
    
    for u in largest_component:
        neighbors = adjacency.get(u, set())
        tmp_set = set()
        for v in neighbors:
            if v in largest_component_set:
                tmp_set.add(v)
        tmp_adjacency[u] = tmp_set


    _, avg_clustering = compute_local_metrics(tmp_adjacency)

    log_result("Average clustering coefficient of largest weak component", round(avg_clustering, 5), file)

def graph_analysis_5(file):
    _, nodes, adjacency = load_graph(file)

    degrees = [len(adjacency.get(node, [])) for node in nodes]
    if not degrees:
        return

    min_deg = min(degrees)
    max_deg = max(degrees)
    avg_deg = sum(degrees) / len(degrees)

    log_result("Minimum degree", min_deg, file)
    log_result("Maximum degree", max_deg, file)
    log_result("Average degree", round(avg_deg, 5), file)

def graph_analysis(file, x_values=range(10, 101, 10)):
    _, nodes, adjacency = load_graph(file)
    nodes = list(nodes)
    visited = set()
    max_component = []
    for node in nodes:
        if node not in visited:
            component = []
            dfs(adjacency, node, visited, component)
            if len(component) > len(max_component):
                max_component = component

    original_largest_component_size = len(max_component)

    random_results = []
    degree_based_results = []

    for x in x_values:
        num_remove = int(len(nodes) * x / 100)

        random_nodes = nodes.copy()
        random.shuffle(random_nodes)
        removed_random = random_nodes[:num_remove]
        new_nodes_rnd, new_adj_rnd = remove_nodes(removed_random, adjacency)
        largest_component_size_rnd = get_largest_weak_component_size_absolute(new_nodes_rnd, new_adj_rnd, original_largest_component_size)
        random_results.append(largest_component_size_rnd)

        degree_sorted = sorted(nodes, key=lambda n: len(adjacency.get(n, [])), reverse=True)
        removed_top = degree_sorted[:num_remove]
        new_nodes_deg, new_adj_deg = remove_nodes(removed_top, adjacency)
        largest_component_size_deg = get_largest_weak_component_size_absolute(new_nodes_deg, new_adj_deg, original_largest_component_size)
        degree_based_results.append(largest_component_size_deg)

        log_result(f"Random removal {x}%", largest_component_size_rnd, file)
        log_result(f"Degree-based removal {x}%", largest_component_size_deg, file)

def run_all_graph_analyses(file, directed=False):
    graph_analysis_1(file, directed)
    graph_analysis_2(file)
    graph_analysis_3(file)
    graph_analysis_4(file)
    graph_analysis_5(file)
    graph_analysis(file)
    save_results_to_csv()

run_all_graph_analyses("data/digraph_1.txt", directed=True)
run_all_graph_analyses("data/digraph_2.txt", directed=True)
run_all_graph_analyses("data/digraph_3.txt", directed=True)

run_all_graph_analyses("data/graph_0.txt", directed=False)
run_all_graph_analyses("data/graph_1.txt", directed=False)
run_all_graph_analyses("data/graph_2.txt", directed=False)
run_all_graph_analyses("data/graph_3.txt", directed=False)
run_all_graph_analyses("data/graph_4.txt", directed=False)
run_all_graph_analyses("data/graph_5.txt", directed=False)
run_all_graph_analyses("data/graph_6.txt", directed=False)
run_all_graph_analyses("data/graph_7.txt", directed=False)
run_all_graph_analyses("data/graph_8.txt", directed=False)
run_all_graph_analyses("data/CA-AstroPh.txt", directed=False)
run_all_graph_analyses("data/CA-GrQc.txt", directed=False)
run_all_graph_analyses("data/Email-EuAll.txt", directed=False)
run_all_graph_analyses("test_graph.txt", directed=False)
