def degree_stats_and_distribution(graph: dict[int, set[int]]):
    degrees = [len(neighbors) for neighbors in graph.values()]

    min_deg = min(degrees)
    max_deg = max(degrees)
    avg_deg = sum(degrees) / len(degrees)

    # Распределение степеней
    degree_count = {}
    for d in degrees:
        if d in degree_count:
            degree_count[d] += 1
        else:
            degree_count[d] = 1

    total_nodes = len(graph)
    degree_prob = {}
    for k, v in degree_count.items():
        degree_prob[k] = v / total_nodes

    return min_deg, max_deg, avg_deg, degree_prob


