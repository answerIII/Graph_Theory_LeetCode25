import csv

results_log = {}

def log_result(metric_name, value, graph_name="graph"):
    if metric_name not in results_log:
        results_log[metric_name] = {}
    results_log[metric_name][graph_name] = value

def save_results_to_csv(filename="graph_testing_results.csv"):
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

def load_graph(file, directed=False):
    edges = []
    nodes = set()
    adjacency = {}

    with open(file, 'r') as f:
        for line in f:
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

def reverse_graph(edges):
    reversed_graph = {}
    for u, v in edges:
        if v not in reversed_graph:
            reversed_graph[v] = set()
        reversed_graph[v].add(u)
    return reversed_graph

def bfs(adjacency, start):
    dist = {start: 0}
    queue = [start]
    while queue:
        node = queue.pop(0)
        for neighbor in adjacency.get(node, []):
            if neighbor not in dist:
                dist[neighbor] = dist[node] + 1
                queue.append(neighbor)
    return dist

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

def count_links(adjacency, node):
    neighbors = list(adjacency.get(node, set()))
    links = 0
    for i in range(len(neighbors)):
        for j in range(i + 1, len(neighbors)):
            if neighbors[j] in adjacency.get(neighbors[i], set()):
                links += 1
    return links

def count_triples(adjacency):
    triples = 0.0
    for neighbors in adjacency.values():
        k = len(neighbors)
        triples += k * (k - 1) / 2
    return triples

def compute_triangles(adjacency):
    triangles = 0
    for node in adjacency:
        triangles += count_links(adjacency, node)
    return triangles // 3

def analyze_graph(file, team_number=3):
    directed = "digraph" in file
    edges, nodes, adjacency = load_graph(file, directed=directed)
    num_nodes = len(nodes)
    num_edges = len(edges)
    max_edges = num_nodes * (num_nodes - 1)
    if not directed:
        max_edges //= 2
    density = num_edges / max_edges if max_edges else 0

    visited = set()
    components = []
    for node in nodes:
        if node not in visited:
            component = []
            dfs(adjacency, node, visited, component)
            components.append(component)

    largest_component = max(components, key=len) if components else []
    share_in_largest_component = len(largest_component)

    diameter = 0
    radius = float('inf')
    if file != "data/graph_2.txt" and not directed:
        for node in largest_component:
            dist = bfs(adjacency, node)
            eccentricity = max(dist.get(n, 0) for n in largest_component)
            diameter = max(diameter, eccentricity)
            radius = min(radius, eccentricity)

    num_triangles = compute_triangles(adjacency)

    neighbors = adjacency.get(team_number, set())
    k = len(neighbors)
    if k < 2:
        clustering = 0.0
    else:
        links = count_links(adjacency, team_number)
        clustering = (2 * links) / (k * (k - 1))

    max_node = max(nodes)
    dist = bfs(adjacency, max_node)
    distance_to_team_node = dist.get(team_number, None)

    if not directed:
        log_result("Number of nodes", num_nodes, file)
        log_result("Number of edges", num_edges, file)
        if diameter == 0 and not directed:
            log_result("Diameter", "N/A", file)
            log_result("Radius", "N/A", file)
        else:
            log_result("Diameter", diameter, file)
            log_result("Radius", radius, file)
        log_result("Graph density", round(density, 5), file)
        log_result("Size of largest weakly connected component", share_in_largest_component, file)
        log_result("Number of triangles", num_triangles, file)
        log_result(f"Clustering coefficient of node {team_number}", round(clustering, 5), file)
        log_result(f"Distance from \"max\" node to node {team_number}", distance_to_team_node, file)
    else:
        log_result("Number of nodes", num_nodes, file)
        log_result("Number of edges", num_edges, file)
        log_result("Graph density", round(density, 5), file)
        log_result("Size of largest weakly connected component", share_in_largest_component, file)
        log_result(f"Distance from \"max\" node to node {team_number}", distance_to_team_node, file)
        reversed_adj = reverse_graph(edges)
        visited = set()
        post_order = []
        for node in nodes:
            if node not in visited:
                dfs(reversed_adj, node, visited, post_order)

        visited = set()
        strong_components = []
        for node in reversed(post_order):
            if node not in visited:
                component = []
                dfs(adjacency, node, visited, component)
                strong_components.append(component)

        num_strong_components = len(strong_components)
        largest_strong = max((len(c) for c in strong_components), default=0)
        share_largest_strong = round(largest_strong / num_nodes, 5) if num_nodes else 0.0

        log_result("Number of strongly connected components", num_strong_components, file)
        log_result("Share of nodes in largest strong component", share_largest_strong, file)

for i in range(9):
    analyze_graph(f"data/graph_{i}.txt", team_number=2)
for i in range(1, 4):
    analyze_graph(f"data/digraph_{i}.txt", team_number=2)

save_results_to_csv()
