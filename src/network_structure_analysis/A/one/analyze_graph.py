from collections import deque

def read_graph(file_path, directed=False):
    graph = {}
    if file_path[-4:] == '.csv':
        separator = ','
    else:
        separator = ' '
    with open(file_path, 'r') as f:
        line = f.readline().strip()
        while line.startswith('#'):
            line = f.readline().strip()
        n, m = map(int, line.split(separator))
        for i in range(n):
            graph[i] = set()
        for _ in range(m):
            u, v = map(int, f.readline().split(separator))
            graph[u].add(v)
            if not directed:
                graph[v].add(u)
    return graph, n, m


def calculate_density(n, m, directed):
    if n < 2:
        return 0.0
    if directed:
        return m / (n * (n - 1))
    else:
        return 2 * m / (n * (n - 1))

def weak_components(graph, n):
    visited = set()
    components = []
    for node in range(n):
        if node not in visited:
            queue = deque([node])
            visited.add(node)
            component = []
            while queue:
                current = queue.popleft()
                component.append(current)
                for neighbor in graph.get(current, set()):
                    if neighbor not in visited:
                        visited.add(neighbor)
                        queue.append(neighbor)
            components.append(component)
    return components

def kosaraju(graph, n):
    visited = set()
    order = []
    def dfs(u):
        stack = [(u, False)]
        while stack:
            node, processed = stack.pop()
            if processed:
                order.append(node)
                continue
            if node in visited:
                continue
            visited.add(node)
            stack.append((node, True))
            for v in graph.get(node, set()):
                if v not in visited:
                    stack.append((v, False))
    for u in range(n):
        if u not in visited:
            dfs(u)
    transposed = {i: set() for i in range(n)}
    for u in graph:
        for v in graph[u]:
            transposed[v].add(u)
    visited.clear()
    components = []
    for u in reversed(order):
        if u not in visited:
            stack = [u]
            visited.add(u)
            component = []
            while stack:
                node = stack.pop()
                component.append(node)
                for v in transposed.get(node, set()):
                    if v not in visited:
                        visited.add(v)
                        stack.append(v)
            components.append(component)
    return components

def analyze_network(file_path, directed=False):
    graph, n, m = read_graph(file_path, directed)
    density = calculate_density(n, m, directed)
    weak_comp = weak_components(graph, n)
    max_weak = max(len(c) for c in weak_comp) if weak_comp else 0
    weak_ratio = max_weak / n if n > 0 else 0
    strong_comp = []
    strong_ratio = 0
    if directed:
        strong_comp = kosaraju(graph, n)
        max_strong = max(len(c) for c in strong_comp) if strong_comp else 0
        strong_ratio = max_strong / n if n > 0 else 0
    return {
        'vertices': n,
        'edges': m,
        'density': round(density, 6),
        'weak_components': len(weak_comp),
        'max_weak_ratio': round(weak_ratio, 4),
        'strong_components': len(strong_comp) if directed else None,
        'max_strong_ratio': round(strong_ratio, 4) if directed else None
    }
