"""функция для чтения графа и файла и сохранения его"""
import networkx as nx # библиотека на Python для работы с графами
import collections

def load_graph_from_file(filename: str, directed: bool = True):
    """ граф ( словарь словарей ) будет храниться следующим образом:
    G[node1] — это все исходящие соседи узла node1
    G[node1][neighbor] — это словарь с атрибутами ребра (если есть, например, веса)"""
    if directed:
        G = nx.DiGraph()
    else:
        G = nx.Graph()

    with open(filename, 'r') as f:
        for line in f:
            if line.startswith('#') or not line.strip():
                continue  # пропускаем комментарии и пустые строки
            
            parts = line.strip().split()
            if len(parts) != 2:
                continue  # пропускаем некорректные строки

            u, v = map(int, parts)
            G.add_edge(u, v)

    #return G возвращаем обьект бибилиотеки для того чтобы проверить какой результат правильный
    return nx.to_dict_of_lists(G), G

def dfs_iterative(graph: dict, visited: set, start):
    """возращаем компоненту которую обошли"""
    visited_vertices = set()
    stack : list = []
    stack.append(start)
    while stack:
        v = stack.pop()
        if v not in visited:
            visited_vertices.add(v)
            visited.add(v)
            for neighbor in graph[v]:
                stack.append(neighbor)
    return visited_vertices
                
def to_undirected(graph: dict[int, list[int]]) -> dict[int, set[int]]:
    """орграф -> неорграф"""
    undirected_graph = {}

    for u in graph:
        if u not in undirected_graph:
            undirected_graph[u] = set()
        for v in graph[u]:
            undirected_graph[u].add(v)
            if v not in undirected_graph:
                undirected_graph[v] = set()
            undirected_graph[v].add(u)

    return undirected_graph
                
def bfs(graph: dict[int, set[int]], start: int, component: set[int] = None) -> dict[int, int]:
    distances = {start: 0}
    queue = collections.deque([start])
    while queue:
        node = queue.popleft()
        for neighbor in graph.get(node, set()):
            if component is None or neighbor in component:
                if neighbor not in distances:
                    distances[neighbor] = distances[node] + 1
                    queue.append(neighbor)
    return distances