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
            line = line.strip()
            if not line or line.startswith('#'):
                continue  # пропускаем пустые строки и комментарии
            parts = line.replace(',', ' ').split()
            try:
                u, v = map(int, parts[:2])
            except ValueError:
                # Если не получилось привести к int — скорее всего заголовок, пропускаем
                continue
            if u != v:  # фильтруем петли
                G.add_edge(u, v)
            else:
                G.add_node(u)
    #return G возвращаем обьект бибилиотеки для того чтобы проверить какой результат правильный
    dict_of_lists = nx.to_dict_of_lists(G)
    dict_of_sets = dict()
    for node, neighbors in dict_of_lists.items():
        dict_of_sets[node] = set(neighbors)
    return nx.to_dict_of_lists(G), G

def load_large_graph_from_file(filename: str, max_vertices: int = 100000):
    """ граф ( словарь словарей ) будет храниться следующим образом:
    G[node1] — это все исходящие соседи узла node1
    G[node1][neighbor] — это словарь с атрибутами ребра (если есть, например, веса)"""
    G = nx.Graph()
    selected_vertices = set()

    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.replace(',', ' ').split()
            if len(parts) < 2:
                continue
            try:
                u, v = map(int, parts[:2])
            except ValueError:
                continue

            # наращиваем множество вершин, если ещё не достигли лимита
            if len(selected_vertices) < max_vertices:
                selected_vertices.add(u)
                selected_vertices.add(v)
            else:
                break

            G.add_edge(u, v)

    dict_of_lists = nx.to_dict_of_lists(G)
    dict_of_sets = dict()
    for node, neighbors in dict_of_lists.items():
        dict_of_sets[node] = set(neighbors)
    return dict_of_sets

def dfs(graph: dict, visited: set, start):
    """простой обход"""
    stack  = [start]
    component = set()
    while stack:
        v = stack.pop()
        if v not in visited:
            visited.add(v)
            component.add(v)
            for neighbor in graph[v]:
                stack.append(neighbor)
    return component

def dfs_iterative_with_component(graph: dict, visited: set, start):
    """возращаем компоненту которую обошли"""
    visited_vertices = set()
    stack  = [start]
    while stack:
        v = stack.pop()
        if v not in visited:
            visited_vertices.add(v)
            visited.add(v)
            for neighbor in graph[v]:
                stack.append(neighbor)
    return visited_vertices

def dfs_iterative_with_time_out(graph: dict, visited: set, start, posled: list):
    """запоминаем в каком порядке были обработаны вершины"""
    stack = [(start, False)]
    while stack:
        v, processed = stack.pop()
        if not processed:
          if v not in visited:
            visited.add(v)
            stack.append((v, True))
            for neighbor in reversed(graph.get(v, [])):
                if neighbor not in visited:
                    stack.append((neighbor,False))
        else:
            posled.append(v)
                
def to_undirected(graph: dict) -> dict[int, list[int]]:
    """орграф -> неорграф"""
    undirected_graph = dict()
    for u in graph:
        if u not in undirected_graph:
            undirected_graph[u] = []
        for v in graph[u]:
            if v not in undirected_graph[u]:
                undirected_graph[u].append(v)
            if v not in undirected_graph:
                undirected_graph[v] = []
            if u not in undirected_graph[v]:
                undirected_graph[v].append(u)

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
   
def bfs_with_path(graph: dict[int, set[int]], start: int, end: int):
    """бфс с запоминанием пути"""
    queue = collections.deque([start])
    parents = dict()
    parents[start] = None
    while queue:
        node = queue.popleft()
        if node == end:
            break
        for neighbor in graph.get(node, set()):
                if neighbor not in parents:
                    queue.append(neighbor)
                    parents[neighbor] = node
    
    if end not in parents and start!=end :
        return None
                     
    path = [end]
    cur_node = end
    while cur_node!= start:
        cur_node = parents[cur_node]
        path.append(cur_node)
    return path

def bfs_with_path_and_distance(graph: dict[int, set[int]], start: int) -> tuple[dict, dict]:
    """бфс с построением дерева кратчайших путей.
    Возвращает:
    - parents: родитель каждой вершины в дереве (None для start),
    - distances: расстояние от start до каждой вершины."""
    distances = {start: 0}
    queue = collections.deque([start])
    parents = dict()
    parents[start] = None
    while queue:
        node = queue.popleft()
        for neighbor in graph.get(node, set()):
                if neighbor not in parents:
                    distances[neighbor] = distances[node] + 1
                    queue.append(neighbor)
                    parents[neighbor] = node
    return parents, distances


def reverse_graph(graph: dict):
    reversed_graph = {}
    for v in graph:
        for neighbor in graph[v]:
            if neighbor not in reversed_graph:
                reversed_graph[neighbor]= []
            reversed_graph[neighbor].append(v)
    for v in graph:
        if v not in reversed_graph:
            reversed_graph[v] = []
    return reversed_graph
