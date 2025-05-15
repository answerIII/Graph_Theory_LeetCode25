"""функция для чтения графа и файла и сохранения его"""
import networkx as nx # библиотека на Python для работы с графами

def load_graph_from_file(filename: str, directed: bool = True) -> dict:
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
    return nx.to_dict_of_lists

def dfs(graph: dict):
    return