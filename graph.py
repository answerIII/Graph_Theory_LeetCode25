import os
from collections import defaultdict, deque
import numpy as np
from random import sample, choice
from multiprocessing import Pool
import tqdm
import pickle


np.random.seed(26)


def make_graph(path, type='txt'):
    # Создаем словарь, который по умолчанию будет присваивать ключу пустое множество
    graph = defaultdict(set)
    unique_edges = 0
    all_edges = 0

    try:
        with open(path, 'r') as file:
            if type == 'txt':
                for line in file:
                    # Убираем лишние символы в начале и конце строки
                    line = line.strip()

                    # Пропускаем комментарии
                    if line.startswith('#') or not line:
                        continue

                    u, v = line.split()
                    u = int(u)
                    v = int(v)

                    # Считаем общее количество ребер (включая кратные)
                    all_edges += 1

                    # Если ребро (u, v) ещё не было добавлено в словарь, добавляем (u, v) и (v, u)
                    if v not in graph[u]:

                        graph[u].add(v)
                        graph[v].add(u)

                        # Считаем уникальные ребра
                        unique_edges += 1
            elif type == 'mtx':
                lines = file.readlines()

                # Считаем заголовок и проверим наличие параметра симметричности
                # Файл сохраняет граф как матрицу, если она симметрична - граф неориентированный
                head = lines.pop(0)
                symmetric = 'symmetric' in head

                # Считаем следующую строку, чтобы получить число ребер
                _, _, all_edges = [int(i) for i in lines.pop(0).split()]

                for line in lines:
                    line = line.strip()

                    u, v = line.split()
                    u = int(u)
                    v = int(v)

                    if v not in graph[u]:
                        graph[u].add(v)
                        if symmetric:
                            graph[v].add(u)

                        unique_edges += 1
            elif type == 'csv':
                lines = file.readlines()
                lines.pop(0)
                for line in lines:
                    data = list(map(int, line.strip().split(',')))
                    u = data[0]
                    v = data[1]
                    all_edges += 1

                    if v not in graph[u]:
                        graph[u].add(v)
                        graph[v].add(u)

                        unique_edges += 1


    except FileNotFoundError:
        print(f"Ошибка: файл {path} не найден.")
        return None

    except Exception as e:
        print(f"Произошла ошибка при обработке файла: {e}")
        return None

    return  dict(graph), unique_edges, all_edges


def dfs_stack(graph):

    visited = set()
    components = []

    for u in graph:

        if u not in visited:
            stack = [u]
            component = []

            while stack:
                cur = stack.pop()

                if cur not in visited:
                    visited.add(cur)
                    component.append(cur)

                    for v in graph[cur]:
                        if v not in visited:
                            stack.append(v)

            components.append(component)
    return components


def subgraph(components, graph):
    comp = max(components, key=len)
    nodes = set(comp)
    ans = {}

    # Переносим в ответ данные о тех ребрах, которые соединяют вершины наибольшей компоненты слабой связности
    for node in comp:
        ans[node] = {i for i in graph.get(node, set()) if i in nodes}

    return ans


def bfs_far(graph, u):

    dist = {u: 0}
    queue = deque([u])
    u = u
    max_dist = 0

    while queue:

        cur = queue.popleft()

        for v in graph.get(cur, set()):
            if v not in dist:

                dist[v] = dist[cur] + 1
                queue.append(v)
                if dist[v] > max_dist:
                    u = v
                    max_dist = dist[v]

    return u, dist


def dbl_swp_diam(graph):
    if not graph:
        return 0

    u = choice(list(graph))
    v, _ = bfs_far(graph, u)
    _, ans = bfs_far(graph, v)

    return max(ans.values())


def percentile90(graph, n=500):

    nodes = list(graph.keys()) if hasattr(graph, 'keys') else list(graph)

    distances = []

    for _ in range(n):
        # Выберем две случайные вершины
        u, v = sample(nodes, 2)

        # Посчитаем расстояние с помощью BFS
        _, d = bfs_far(graph, u)

        if v in d:
            distances.append(d[v])

    # Используем функцию из библиотеки numpy, чтобы найти n-процентиль выборки
    return int(np.percentile(distances, 90)) if distances else 0


def snowball(graph, n=500):
    if not graph: return {}

    # Выбираем случаные вершины, с которых начнем
    s = sample(list(graph), min(3, len(graph)))
    visited, queue = set(s), deque(s)

    # Будем продолжать, пока не наберем n соседей или пока не иссякнет очередь
    while queue and len(visited) < n:
        c = queue.popleft()
        for i in graph.get(c, []):
            if i not in visited and len(visited) < n:
                visited.add(i)
                queue.append(i)

    sg = {node: set() for node in visited}
    for node in visited:
        sg[node] = {nb for nb in graph.get(node, []) if nb in visited}

    return sg


def snowball_analysis(graph, n=500):
    subgraph = snowball(graph, n)
    diameter = dbl_swp_diam(subgraph)
    percentile = percentile90(subgraph, n=100)
    return diameter, percentile


def triangles(graph):

    triangles = 0

    for u in graph:
        current = graph[u]
        for v in current:
            # Анализируем соседей
            if v > u:
                for w in current & graph[v]:
                    if w > v:
                        triangles += 1

    return triangles


def node_clust(graph, n):
    nodes = graph.get(n, set())
    neighbours = len(nodes)
    if neighbours < 2: return 0.0

    max = neighbours * (neighbours - 1) / 2
    unique = 0

    for u in nodes:
        for v in nodes:
            if u > v and v in graph.get(u, set()):
                unique += 1

    return unique / max


def avg_clust(graph):

    ans = 0.0
    count = 0

    for node in graph:
        coeff = node_clust(graph, node)
        ans += coeff
        count += 1

    return ans / count


def gcc(g):
    total, count = 0, 0

    for u in g:
        n = g[u]
        k = len(n)

        if k < 2:
            continue
        total += k * (k - 1) / 2

        for v in n:
            for w in n:
                if v > w and w in g.get(v,set()):
                    count += 1

    return count / total if total else 0.0


def calculate_node_degrees(graph):
    degrees = [len(u) for u in graph.values()]

    return min(degrees),max(degrees), sum(degrees) / len(degrees)


def analyze_component_resilience(graph, x_values, num_trials=5):
    components = dfs_stack(graph)
    lcc = subgraph(components, graph)
    results = defaultdict(list)
    lcc_nodes = set(lcc.keys())
    original_size = len(lcc)

    for x in x_values:
        for _ in range(num_trials):
            # Создаем копию только для узлов LCC
            current_lcc = {node: set(neighbors) for node, neighbors in lcc.items()}

            # Вычисляем количество узлов для удаления
            num_to_remove = int(original_size * x / 100)

            # Случайно выбираем узлы только из LCC
            nodes_to_remove = sample(list(lcc_nodes), num_to_remove)

            # Удаляем узлы из LCC
            for node in nodes_to_remove:
                # Удаляем связи с соседями
                for neighbor in current_lcc.get(node, set()).copy():
                    current_lcc[neighbor].discard(node)
                del current_lcc[node]

            # Находим новые компоненты в измененной LCC
            new_components = get_weakly_connected_components(current_lcc)

            if new_components:
                new_lcc_size = len(max(new_components, key=len))
                remaining_nodes = sum(len(comp) for comp in new_components)
                results[x].append(new_lcc_size / remaining_nodes if remaining_nodes > 0 else 0)

    # Усредняем результаты
    return {x: sum(results[x]) / len(results[x]) for x in results}


def get_weakly_connected_components(graph_dict):
    visited = set()
    components = []

    for node in graph_dict:
        if node not in visited:
            # BFS для нахождения компоненты
            queue = [node]
            visited.add(node)
            component = []

            while queue:
                current = queue.pop(0)
                component.append(current)

                for neighbor in graph_dict.get(current, set()):
                    if neighbor not in visited:
                        visited.add(neighbor)
                        queue.append(neighbor)

            components.append(component)

    return components


def analyze_targeted_removal(graph, x_values):
    results = {}
    components = dfs_stack(graph)
    lcc = subgraph(components, graph)
    original_size = len(lcc)

    # Предварительно сортируем узлы по степени (по убыванию)
    sorted_nodes = sorted(lcc.items(),
                         key=lambda item: len(item[1]),
                         reverse=True)

    for x in x_values:
        # Создаем копию только для узлов LCC
        current_lcc = {node: set(neighbors) for node, neighbors in lcc.items()}

        # Вычисляем количество узлов для удаления
        num_to_remove = int(original_size * x / 100)

        # Выбираем топ-N узлов с наибольшей степенью
        nodes_to_remove = [node for node, _ in sorted_nodes[:num_to_remove]]

        # Удаляем узлы из LCC
        for node in nodes_to_remove:
            # Удаляем связи с соседями
            for neighbor in current_lcc.get(node, set()).copy():
                current_lcc[neighbor].discard(node)
            del current_lcc[node]

        # Находим новые компоненты в измененной LCC
        new_components = get_weakly_connected_components(current_lcc)

        if new_components:
            new_lcc_size = len(max(new_components, key=len))
            remaining_nodes = sum(len(comp) for comp in new_components)
            results[x] = new_lcc_size / remaining_nodes if remaining_nodes > 0 else 0

    return results


def process_directed_graph_file(path, type='txt'):
    graph = defaultdict(set)

    unique_edges = 0
    all_edges = 0

    try:
        with open(path, 'r') as file:
            if type == 'txt':
                for line in file:
                    line = line.strip()

                    if line.startswith('#') or not line:
                        continue

                    u, v = line.split()
                    u = int(u)
                    v = int(v)


                    all_edges += 1

                    if u not in graph[v]:
                        graph[u].add(v)
                        unique_edges += 1
            elif type == 'mtx':
                lines = file.readlines()

                head = lines.pop(0)
                symmetric = 'symmetric' in head

                _, _, all_edges = [int(i) for i in lines.pop(0).split()]

                for line in lines:
                    line = line.strip()

                    u, v = line.split()
                    u = int(u)
                    v = int(v)

                    if v not in graph[u]:
                        graph[u].add(v)
                        if symmetric:
                            graph[v].add(u)

                        unique_edges += 1
            elif type == 'csv':
                lines = file.readlines()
                lines.pop(0)
                for line in lines:
                    data = list(map(int, line.strip().split(',')))
                    u = data[0]
                    v = data[1]
                    all_edges += 1

                    if v not in graph[u]:
                        graph[u].add(v)

                        unique_edges += 1

    except FileNotFoundError:
        print(f"ошибка: файл {path} не найден.")
        return None

    except Exception as e:
        print(f"произошла ошибка при обработке файла: {e}")
        return None

    return dict(graph), unique_edges, all_edges


def count_scc(graph):
    # Первый проход DFS для определения порядка завершения
    visited = set()
    order = []

    for u in graph:
        if u not in visited:
            stack = [(u, False)]

            while stack:
                cur, processed = stack.pop()
                if processed:
                    order.append(cur)
                    continue
                if cur in visited:
                    continue

                if cur not in visited:
                    visited.add(cur)
                    stack.append((cur, True))
                    for v in graph.get(cur, set()):
                        if v not in visited:
                            stack.append((v, False))

    # Инвертируем граф
    reversed_graph = defaultdict(set)
    for src in graph:
        for dst in graph[src]:
            reversed_graph[dst].add(src)

    # Второй проход DFS в обратном порядке по инвертированному графу
    visited = set()
    components = []

    for u in reversed(order):
        if u not in visited:
            stack = [u]
            visited.add(u)
            component = []

            while stack:
                cur = stack.pop()
                component.append(cur)

                for v in reversed_graph.get(cur, set()):
                    if v not in visited:
                        stack.append(v)
                        visited.add(v)

            components.append(component)
    return components


def spt(graph, u):
    tree = {u: 0}
    queue = deque([u])

    while queue:
        cur = queue.popleft()

        for v in graph.get(cur, set()):
            if v not in tree:
                tree[v] = tree[cur] + 1
                queue.append(v)
    return tree


def get_path(graph, start, goal):
    visited = {start}
    queue = deque([[start]])

    while queue:
        cur = queue.popleft()
        last = cur[-1]
        for v in graph.get(last, set()):
            if v == goal:
                return cur + [v]
            if v not in visited:
                visited.add(v)
                queue.append(cur + [v])
    return []


def process_pair(pair):
    start, goal = pair
    path = get_path(graph, start, goal)
    coverage = {}
    for node in path:
        coverage[node] = 1
    return coverage


def compute_spt(node):
    return node, spt(graph, node)


def get_landmarks(graph, n=16, mode='degree', num_workers=1):
    shortest_path_trees = dict()
    landmarks = []

    if mode == 'degree':
        high_degree_nodes = [node for node, _ in sorted(graph.items(), key=lambda item: len(item[1]), reverse=True)[:n]]
        landmarks.extend(high_degree_nodes)
    elif mode == 'random':
        while n:
            node = choice(list(graph.keys()))
            if node not in landmarks:
                landmarks.append(node)
                n -= 1
    elif mode == 'best-coverage':
        pairs = [sample(list(graph.keys()), 2) for _ in range(n * 50)]

        with Pool(num_workers) as pool:
            results = list(tqdm.tqdm(pool.imap(process_pair, pairs), 
                          total=len(pairs),
                          desc="Processing path coverage"))
        
        merged_coverage = {}
        for cov in results:
            for node in cov:
                if node in merged_coverage:
                    merged_coverage[node] += cov[node]
                else:
                    merged_coverage[node] = cov[node]        
        nodes = sorted(merged_coverage.items(), key=lambda x: x[1], reverse=True)
        landmarks = [node for node, _ in nodes[:n]]
    
    with Pool(num_workers) as pool:
        spt_results = list(tqdm.tqdm(pool.imap(compute_spt, landmarks),
                              total=len(landmarks),
                              desc="Computing shortest path trees"))
    
    shortest_path_trees = dict(spt_results)

    return shortest_path_trees


def landmarks_basic(landmarks, start, goal):
    return max(abs(landmarks[L][start] - landmarks[L][goal]) for L in landmarks)


def landmarks_bfs(graph, landmarks, start, goal):
    nodes = set()
    for landmark in landmarks:
        path = get_path(graph, landmark, start)
        nodes.update(path)
        path = get_path(graph, landmark, goal)
        nodes.update(path)
    subgraph = dict()
    for node in nodes:
        subgraph[node] = {i for i in graph.get(node, set()) if i in nodes}
    # Вернем длину пути как число вершин - 1
    return len(get_path(subgraph, start, goal)) - 1


def bfs_dist(graph, u, goal):
    dist = {u: 0}
    queue = deque([u])

    while queue:
        cur = queue.popleft()
        if cur == goal:
            return u, dist[cur]
        for v in graph.get(cur, set()):
            if v not in dist:
                dist[v] = dist[cur] + 1
                queue.append(v)

    return u, float('inf')


def process_pair_dist(pair):
    start, goal = pair
    distance = bfs_dist(graph, start, goal)[1]
    basic_distance = landmarks_basic(landmarks, start, goal)
    bfs_distance = landmarks_bfs(graph, landmarks, start, goal)
    print(distance, basic_distance, bfs_distance)
    basic_correct = 1 if distance == basic_distance else 0
    bfs_correct = 1 if distance == bfs_distance else 0
    
    return basic_correct, bfs_correct


def evaluate_accuracy(graph, landmarks, n=10, num_workers=None):
    pairs = [sample(list(graph.keys()), 2) for _ in range(n)]
    
    # Параллельная обработка пар
    with Pool(num_workers) as pool:
        results = list(tqdm.tqdm(
            pool.imap(process_pair_dist, pairs),
            total=len(pairs),
            desc="Evaluating accuracy"
        ))
    
    # Суммируем результаты
    basic_right = sum(res[0] for res in results)
    bfs_right = sum(res[1] for res in results)
    
    bfs_accuracy = bfs_right / n  
    basic_accuracy = basic_right / n
    
    return bfs_accuracy, basic_accuracy


pathname = "/home/eldar/Рабочий стол/Graphs/"
filenames = ['com-youtube-ungraph.txt']

with open(pathname + 'output.txt', 'w') as output:
    for filename in filenames:
        print(filename)

        lcc_filename = pathname + filename[:-4] + "-lcc.pkl"
        if os.path.isfile(lcc_filename):
            print('Loading graph')
            with open(lcc_filename, 'rb') as lcc_file:
                graph = pickle.load(lcc_file)
        else:
            print('Processing graph')
            graph, edges, _ = make_graph(pathname + filename, type=filename.split('.')[-1])
            
            print('Processing components')
            components = dfs_stack(graph)
            
            print('Processing largest component')
            graph = subgraph(components, graph)
            with open(lcc_filename, "wb") as lcc_file:
                pickle.dump(graph, lcc_file)

        n = 32
        mode = 'degree'
        lm_filename = pathname + filename[:-4] + '-' + mode + '-' + str(n) + "-landmarks.pkl"
        landmarks = dict()
        
        if os.path.isfile(lm_filename):
            print('Loading landmarks')
            with open(lm_filename, 'rb') as lm_file:
                landmarks = pickle.load(lm_file)
        else:
            landmarks = get_landmarks(graph, n=n, mode=mode)
            with open(lm_filename, "wb") as lm_file:
                pickle.dump(landmarks, lm_file)
        
        print(evaluate_accuracy(graph, landmarks, 20))
