"""
Module for structural analysis of social networks and graphs.

Implements algorithms to compute key network metrics, visualize properties,
and analyze connectivity patterns. Supports both directed and undirected graphs.
"""

import itertools
import random
from collections import deque

import matplotlib.pyplot as plt
import networkx as nx
import numpy as np


def load_graph(file_path, directed=False):
    """
    Load graph structure from edge list file.

    Parameters:
    -----------
    file_path : str
        Path to text file containing edge pairs
    directed : bool, optional
        Whether to create directed graph (default=False)

    Returns:
    --------
    nx.Graph | nx.DiGraph
        NetworkX graph object representing the network

    Notes:
    ------
    File format expects one edge per line with two node identifiers separated by whitespace
    """
    G = nx.DiGraph() if directed else nx.Graph()
    with open(file_path, "r") as f:
        for line in f:
            if line.strip() == "":
                continue
            parts = line.strip().split()
            u, v = parts[:2]
            G.add_edge(u, v)
    return G


def analyze_graph(G):
    """
    Perform comprehensive analysis of graph structure and properties.

    Parameters:
    -----------
    G : nx.Graph | nx.DiGraph
        Input graph to analyze

    Returns:
    --------
    dict
        Dictionary containing computed metrics with keys:
        - num_nodes: total number of nodes
        - num_edges: total number of edges
        - density: network density
        - [component-related metrics]
        - [path length statistics]
        - [clustering coefficients]
        - [degree statistics]

    Analysis includes:
    ------------------
    1. Basic network statistics
    2. Connectivity components analysis
    3. Path length estimation (diameter, percentiles)
    4. Clustering coefficients calculation
    5. Degree distribution analysis
    6. Visualization of key properties

    Generates visualizations:
    -------------------------
    - degree_distribution.png
    - degree_distribution_log.png
    - clustering_distribution.png
    - distance_distribution.png
    - giant_component.png (if size <=500)
    - component_size_distribution.png
    """
    results = {}

    H = G.to_undirected() if G.is_directed() else G

    """Section A1: basic network statistics"""
    results["num_nodes"] = H.number_of_nodes()
    results["num_edges"] = H.number_of_edges()
    n = results["num_nodes"]
    max_edges = n * (n - 1) // 2
    results["density"] = results["num_edges"] / max_edges if max_edges > 0 else 0.0

    if G.is_directed():
        weak_components = list(nx.connected_components(H))
        results["num_weak_components"] = len(weak_components)
        largest_weak = max(weak_components, key=len, default=set())
        results["largest_weak_ratio"] = len(largest_weak) / n if n > 0 else 0.0

        strong_components = list(nx.strongly_connected_components(G))
        results["num_strong_components"] = len(strong_components)
        largest_strong = max(strong_components, key=len, default=set())
        g_num_nodes = G.number_of_nodes()
        results["largest_strong_ratio"] = (
            len(largest_strong) / g_num_nodes if g_num_nodes > 0 else 0.0
        )
    else:
        components = list(nx.connected_components(H))
        results["num_components"] = len(components)
        largest = max(components, key=len, default=set())
        results["largest_component_ratio"] = len(largest) / n if n > 0 else 0.0

    """Section A2: diameter and 90th percentile for the LWC component"""
    if G.is_directed():
        largest_comp_nodes = largest_weak
    else:
        largest_comp_nodes = largest if "largest_component_ratio" in results else set()

    largest_comp = H.subgraph(largest_comp_nodes) if largest_comp_nodes else nx.Graph()
    if largest_comp.number_of_nodes() == 0:
        results.update(
            {
                "diameter_double_bfs": 0,
                "diameter_random": 0,
                "percentile_90_random": 0,
                "diameter_snowball": 0,
                "percentile_90_snowball": 0,
            }
        )
    else:
        nodes = list(largest_comp.nodes())
        try:
            r = random.choice(nodes)
            dist_r = nx.single_source_shortest_path_length(largest_comp, r)
            a = max(dist_r, key=dist_r.get)
            dist_a = nx.single_source_shortest_path_length(largest_comp, a)
            b = max(dist_a, key=dist_a.get)
            results["diameter_double_bfs"] = dist_a[b]
        except Exception:
            results["diameter_double_bfs"] = 0

        sampled_nodes = random.sample(nodes, min(500, len(nodes)))
        pairs = list(itertools.combinations(sampled_nodes, 2))
        random.shuffle(pairs)
        pairs = pairs[:500]
        distances = []
        for u, v in pairs:
            try:
                d = nx.shortest_path_length(largest_comp, u, v)
                distances.append(d)
            except nx.NetworkXNoPath:
                continue
        results["diameter_random"] = max(distances) if distances else 0
        results["percentile_90_random"] = (
            np.percentile(distances, 90) if distances else 0
        )

        snowball_size = 1000
        if len(nodes) >= snowball_size:
            start = random.sample(nodes, 2)
            snowball = set(start)
            queue = deque(start)
            while queue and len(snowball) < snowball_size:
                current = queue.popleft()
                neighbors = list(largest_comp.neighbors(current))
                for nbr in neighbors:
                    if nbr not in snowball:
                        snowball.add(nbr)
                        queue.append(nbr)
                        if len(snowball) >= snowball_size:
                            break
            snowball_sub = largest_comp.subgraph(snowball)
            snow_nodes = list(snowball_sub.nodes())
            snow_distances = []
            for u, v in itertools.combinations(snow_nodes, 2):
                try:
                    d = nx.shortest_path_length(snowball_sub, u, v)
                    snow_distances.append(d)
                except nx.NetworkXNoPath:
                    continue
            results["diameter_snowball"] = max(snow_distances) if snow_distances else 0
            results["percentile_90_snowball"] = (
                np.percentile(snow_distances, 90) if snow_distances else 0
            )
        else:
            results["diameter_snowball"] = results["diameter_random"]
            results["percentile_90_snowball"] = results["percentile_90_random"]

    """Section A3: triangles and clustering coefficients"""
    try:
        triangles = sum(nx.triangles(H).values()) // 3
    except nx.NetworkXError:
        triangles = 0
    results["num_triangles"] = triangles
    results["avg_clustering"] = nx.average_clustering(H)
    results["global_clustering"] = nx.transitivity(H)

    """Section A4: average clustering coefficient for the LC"""
    if largest_comp.number_of_nodes() > 0:
        clustering = nx.clustering(largest_comp)
        results["avg_clustering_largest"] = sum(clustering.values()) / len(clustering)
    else:
        results["avg_clustering_largest"] = 0

    """Section A5: degree statistics"""
    degrees = [d for _, d in H.degree()]
    results["min_degree"] = min(degrees) if degrees else 0
    results["max_degree"] = max(degrees) if degrees else 0
    results["avg_degree"] = np.mean(degrees) if degrees else 0

    if largest_comp.number_of_nodes() > 0:
        clustering = nx.clustering(largest_comp)
        plt.figure()
        plt.hist(clustering.values(), bins=50, alpha=0.7)
        plt.title("Clustering Coefficient Distribution")
        plt.xlabel("Clustering Coefficient")
        plt.ylabel("Frequency")
        plt.savefig("clustering_distribution.png")
        plt.close()

    if "distances" in locals() and len(distances) > 0:
        plt.figure()
        plt.hist(distances, bins=range(max(distances) + 2), alpha=0.7, density=True)
        plt.title("Path Length Distribution")
        plt.xlabel("Distance")
        plt.ylabel("Probability")
        plt.savefig("distance_distribution.png")
        plt.close()

    if largest_comp.number_of_nodes() <= 500 and largest_comp.number_of_nodes() > 0:
        plt.figure(figsize=(12, 12))
        pos = nx.spring_layout(largest_comp, k=0.15, iterations=20)
        nx.draw(
            largest_comp,
            pos,
            node_size=20,
            alpha=0.6,
            edge_color="gray",
            with_labels=False,
        )
        plt.title("Giant Component Visualization")
        plt.savefig("giant_component.png")
        plt.close()

    if G.is_directed():
        component_sizes = [len(c) for c in weak_components]
    else:
        component_sizes = [len(c) for c in components]

    plt.figure()
    plt.hist(
        component_sizes,
        bins=np.logspace(0, np.log10(max(component_sizes) + 1), 50),
        alpha=0.7,
    )
    plt.xscale("log")
    plt.title("Component Size Distribution")
    plt.xlabel("Component Size (log scale)")
    plt.ylabel("Count")
    plt.savefig("component_size_distribution.png")
    plt.close()

    return results


if __name__ == "__main__":
    G = load_graph("Wiki-Vote.txt", directed=False)
    results = analyze_graph(G)
    for key, value in results.items():
        print(f"{key}: {value}")
