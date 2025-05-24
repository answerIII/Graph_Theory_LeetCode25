import os
from collections import defaultdict, deque, Counter
import numpy as np
from random import randint, random, sample, choice
import matplotlib.pyplot as plt
import heapq
from multiprocessing import Pool, cpu_count
from functools import partial
import tqdm


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


def bfs_far_opt(graph, start):
    """Оптимизированная BFS для вычисления расстояний от стартовой вершины."""
    distances = {start: 0}
    queue = deque([start])
    
    while queue:
        u = queue.popleft()
        for v in graph.get(u, set()):
            if v not in distances:
                distances[v] = distances[u] + 1
                queue.append(v)
    
    farthest_node = max(distances.items(), key=lambda x: x[1])[0]
    return farthest_node, distances


def bfs_tree(graph, u):
    dist = {u: 0}
    queue = deque([u])

    while queue:
        cur = queue.popleft()

        for v in graph.get(cur, set()):
            if v not in dist:
                dist[v] = dist[cur] + 1
                queue.append(v)

    return u, dist


def process_landmark(graph, node):
    # Обработка одной ландмарки для параллельного использования
    return (node, bfs_tree(graph, node))


def get_landmarks(graph, high_degree=8, random_n=8):
    print('Processing landmarks')
    landmarks = {}
    nodes = []

    if high_degree:
        hd_nodes = [node for node, _ in sorted(graph.items(), key=lambda item: len(item[1]), reverse=True)[:high_degree]]
        nodes.extend(hd_nodes)

    while random_n:
        node = choice(list(graph.keys()))
        if node not in nodes:
            nodes.append(node)
            random_n -= 1

    with Pool(processes=4) as pool:
        results = pool.map(partial(bfs_tree, graph), nodes)
    
    landmarks = dict(results)

    return landmarks


'''
def bfs_tree(graph, u):
    # BFS, который не возвращает самую дальнюю вершину
    dist = {u: 0}
    queue = deque([u])

    while queue:
        cur = queue.popleft()

        for v in graph.get(cur, set()):
            if v not in dist:
                dist[v] = dist[cur] + 1
                queue.append(v)

    return u, dist


def process_landmark(graph, node):
    # Обработка одной ландмарки для параллельного использования
    return (node, bfs_tree(graph, node))


def get_landmarks(graph, high_degree=8, random_n=8, parallel=1):
    print('Processing landmarks')
    landmarks = {}
    nodes = []

    if high_degree:
        hd_nodes = [node for node, _ in sorted(graph.items(), key=lambda item: len(item[1]), reverse=True)[:high_degree]]
        nodes.extend(hd_nodes)

    while random_n:
        node = choice(list(graph.keys()))
        # Исключаем дубликаты
        if node not in nodes:
            nodes.append(node)
            random_n -= 1

    # По возможности используем параллельную обработку
    with Pool(processes=parallel) as pool:
        results = pool.map(partial(bfs_tree, graph), nodes)
    
    landmarks = dict(results)

    return landmarks


def a_star(graph, landmarks, start, goal):
    closed = set()
    open = []
    h = lambda u, v : max([abs(landmarks[L][u] - landmarks[L][v]) for L in landmarks.keys()])
    g = {start: 0}
    heapq.heappush(open, (g[start] + h(start, goal), start))

    while open:
        f, u = heapq.heappop(open)
        if u in closed:
            continue
        if u == goal:
            return g[u]
        for v in graph.get(u, set()):
            if (v not in closed) and not (any(v in item for item in open)):
                g[v] = g[u] + 1
                heapq.heappush(open, (g[v] + h(v, goal), v))
    return False


def get_dist(graph, u, v):
    _, dist = bfs_far(graph, u)
    return dist[v]
'''


def a_star(graph, landmarks, start, goal):
    closed = set()
    n = len(graph)
    open = []
    h = lambda u, v : max([abs(landmarks[L][u] - landmarks[L][v]) for L in landmarks.keys()])
    g = {start: 0}
    heapq.heappush(open, (g[start] + h(start, goal), start))

    while open:
        print(f'{(len(closed) / n) * 100:.8f}%')
        f, u = heapq.heappop(open)
        if u in closed:
            continue
        if u == goal:
            return g[u]
        for v in graph.get(u, set()):
            if (v not in closed) and not (any(v in item for item in open)):
                g[v] = g[u] + 1
                heapq.heappush(open, (g[v] + h(v, goal), v))
    return False


def a_star_opt(graph, landmarks, start, goal):
    """Оптимизированная A* с эвристикой на основе контрольных точек."""
    closed = set()
    open_set = []
    g_scores = {start: 0}
    
    # Кэширование эвристик для ускорения
    h_cache = {}
    
    def heuristic(u, v):
        if (u, v) not in h_cache:
            max_diff = 0
            for L in landmarks:
                diff = abs(landmarks[L].get(u, float('inf')) - landmarks[L].get(v, float('inf')))
                if diff > max_diff:
                    max_diff = diff
            h_cache[(u, v)] = max_diff
        return h_cache[(u, v)]
    
    heapq.heappush(open_set, (heuristic(start, goal), start))
    
    while open_set:
        f, u = heapq.heappop(open_set)
        
        if u == goal:
            return g_scores[u]
            
        if u in closed:
            continue
            
        closed.add(u)
        
        for v in graph.get(u, set()):
            if v in closed:
                continue
                
            tentative_g = g_scores[u] + 1  # Предполагаем вес ребра = 1
            
            if v not in g_scores or tentative_g < g_scores[v]:
                g_scores[v] = tentative_g
                f_score = tentative_g + heuristic(v, goal)
                heapq.heappush(open_set, (f_score, v))
    
    return float('inf')  # Путь не найден


def get_dist(graph, u, v):
    _, dist = bfs_far(graph, u)
    return dist[v]


filenames = ['/home/eldar/Рабочий стол/Graphs/ca-coauthors-dblp.txt']

with open('output.txt', 'w') as output:
    for filename in filenames:
        print(filename + '\n')

        print('Processing graph')
        graph, edges, _ = make_graph(filename, type=filename.split('.')[-1])
        
        print('Processing components')
        components = dfs_stack(graph)
        
        print('Processing largest component')
        graph = subgraph(components, graph)   

        '''
        print(str(len(graph)) + '\n')
        print(str(edges) + '\n')
        num_vertex = len(graph)
        max_num_vertex = num_vertex * (num_vertex - 1) // 2
        density = edges / max_num_vertex if max_num_vertex > 0 else 0.0
        output.write(f"Кол-во вершин: {num_vertex}, кол-во ребер: {edges}, плотность: {density}" + '\n')

        components = dfs_stack(graph)
        output.write(f'Число компонент слабой связности в графе равно {len(components)}' + '\n')

        max_size = max(len(c) for c in components)
        fraction = max_size / num_vertex
        output.write(f'Доля вершин в компоненте слабой связности равна {fraction}' + '\n')

        large_comp = subgraph(components, graph)
        output.write(f'Размер самой большой компоненты слабой связности равен {len(large_comp)}' + '\n')

        diameter = dbl_swp_diam(large_comp)
        output.write(f'Диаметр сети, найденный с помощью двух вершин, равен {diameter}' + '\n')

        ans = percentile90(large_comp)
        output.write(f'90-Процентиль расстояний в большой компоненте равен {ans}' + '\n')

        sn_dim, sn_per = snowball_analysis(large_comp)
        output.write(f'90-Процентиль расстояний в снежном коме равен {sn_per}, диаметр равен {sn_dim}' + '\n')

        tri_number = triangles(graph)
        output.write(f'Количество полных подграфов на 3 вершинах равно {tri_number}' + '\n')

        output.write(f'Средний кластерный коэффициент в компоненте составляет: {avg_clust(large_comp)}' + '\n')
        output.write(f'Средний кластерный коэффициент в графе составляет: {avg_clust(graph)}' + '\n')

        output.write(f'Глобальный кластерный коэффициент в графе составляет: {avg_clust(graph)}' + '\n')

        min_deg, max_deg, avg_deg = calculate_node_degrees(graph)
        output.write(f'Минимальная степень вершины в графе равна {min_deg}' + '\n')
        output.write(f'Максимальная степень вершины в графе равна {max_deg}' + '\n')
        output.write(f'Средняя степень вершины в графе равна {avg_deg}' + '\n')

        x_values = [5, 10, 15, 20, 25, 30]
        resilience_results = analyze_component_resilience(graph, x_values)
        output.write("Устойчивость наибольшей компоненты:" + '\n')
        output.write("Процент удаления | Доля вершин в LCC" + '\n')
        output.write("-------------------------------" + '\n')
        for x in sorted(resilience_results):
            output.write(f"{x:15}% | {resilience_results[x]}" + '\n')

        resilience_results = analyze_targeted_removal(graph, x_values)

        output.write("Устойчивость наибольшей компоненты:" + '\n')
        output.write("Процент удаления | Доля вершин в LCC" + '\n')
        output.write("-------------------------------" + '\n')
        for x in sorted(resilience_results):
            output.write(f"{x:15}% | {resilience_results[x]}" + '\n')

        directed_graph, edges, _ = process_directed_graph_file(filename, type=filename.split('.')[-1]) 
        scc = count_scc(graph)
        output.write(f'Число компонент сильной связности в графе равно {len(scc)}' + '\n')

        directed_subgraph = subgraph(scc, directed_graph)
        max_size = len(directed_graph)
        fraction = max_size / num_vertex
        output.write(f'Доля вершин в компоненте слабой связности равна {fraction}' + '\n')
        output.write('\n')
        '''

        landmarks = get_landmarks(graph)
        n = 10
        pairs = [sample(graph.keys(), 2) for _ in range(n)]
        right = 0

        for u, v in tqdm.tqdm(pairs):
            h = a_star_opt(graph, landmarks, u, v)
            if h == get_dist(graph, u, v):
                right += 1
        
        print(f'Correct: {int(right / n * 100)}%')

