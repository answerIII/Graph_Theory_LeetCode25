from collections import deque

from typing import (
    Dict,
    List,
    Tuple,
    Set,
    Deque
)


def getWeakComponentBFS(undir_adj_list: Tuple[Set[int], ...],
                        visited: List[bool],
                        node_ind: int) -> List[int]:
    """
    Visit new weak component of graph, return visited nodes
    """
    visited[node_ind] = True
    nodes_queue: Deque[int] = deque()
    nodes_queue.append(node_ind)
    new_weak_component = [node_ind]
    while nodes_queue:
        new_node_ind = nodes_queue.popleft()
        for neighbour_node_ind in undir_adj_list[new_node_ind]:
            if not visited[neighbour_node_ind]:
                visited[neighbour_node_ind] = True
                nodes_queue.append(neighbour_node_ind)
                new_weak_component.append(neighbour_node_ind)
    return new_weak_component


def getFurthestNodeBFS(undir_adj_list: Tuple[Set[int], ...],
                       start_node_ind: int) -> Tuple[int, int]:
    """
    Returns pair (furthest_node, distance) using given start_node_ind
    """
    furthest_node = start_node_ind
    distance = -1
    visited = [False for _ in range(len(undir_adj_list))]
    nodes_queue: Deque[int] = deque()
    visited[start_node_ind] = True
    nodes_queue.append(start_node_ind)
    while nodes_queue:
        queue_size = len(nodes_queue)
        furthest_node = nodes_queue[0]
        distance += 1
        for _ in range(queue_size):
            new_node_ind = nodes_queue.popleft()
            for neighbour_node_ind in undir_adj_list[new_node_ind]:
                if not visited[neighbour_node_ind]:
                    visited[neighbour_node_ind] = True
                    nodes_queue.append(neighbour_node_ind)
    return (furthest_node, distance)


def updateDistNodeSubsetBFS(undir_adj_list: Tuple[Set[int], ...],
                            distance_triangle: List[List[int]],
                            node_to_ind_map: Dict[int, int],
                            start_node_ind: int,
                            nodes_to_update: int) -> None:
    """
    Update distances from start_node_ind to a subset of specified vertices
    """
    distance = -1
    visited = [False for _ in range(len(undir_adj_list))]
    nodes_queue: Deque[int] = deque()
    visited[start_node_ind] = True
    nodes_queue.append(start_node_ind)
    node1_ind = node_to_ind_map[start_node_ind]
    while nodes_queue and nodes_to_update > 0:
        queue_size = len(nodes_queue)
        distance += 1
        for _ in range(queue_size):
            new_node_ind = nodes_queue.popleft()
            if new_node_ind in node_to_ind_map:
                node2_ind = node_to_ind_map[new_node_ind]
                if node1_ind <= node2_ind:
                    nodes_to_update -= 1
                    distance_triangle[node1_ind][node2_ind -
                                                 node1_ind] = distance
            for neighbour_node_ind in undir_adj_list[new_node_ind]:
                if not visited[neighbour_node_ind]:
                    visited[neighbour_node_ind] = True
                    nodes_queue.append(neighbour_node_ind)


def updateDistBFS(undir_adj_list: Tuple[Set[int], ...],
                  distance_triangle: List[List[int]],
                  start_node_ind: int,
                  nodes_to_update: int) -> None:
    """
    Update distances from start_node_ind to all other nodes of undir_adj_list
    """
    distance = -1
    visited = [False for _ in range(len(undir_adj_list))]
    nodes_queue: Deque[int] = deque()
    visited[start_node_ind] = True
    nodes_queue.append(start_node_ind)
    while nodes_queue and nodes_to_update > 0:
        queue_size = len(nodes_queue)
        distance += 1
        for _ in range(queue_size):
            new_node_ind = nodes_queue.popleft()
            if start_node_ind <= new_node_ind:
                nodes_to_update -= 1
                distance_triangle[start_node_ind][new_node_ind -
                                                  start_node_ind] = distance
            for neighbour_node_ind in undir_adj_list[new_node_ind]:
                if not visited[neighbour_node_ind]:
                    visited[neighbour_node_ind] = True
                    nodes_queue.append(neighbour_node_ind)


def launchNextBFSIteration(undir_adj_list: Tuple[Set[int], ...],
                           calc_dist_list: List[Dict[int, int]],
                           nodes_queue_list: List[Deque[int]],
                           start_node_ind: int,
                           curr_dist: int) -> None:
    curr_calc_dist_dict = calc_dist_list[start_node_ind]
    curr_queue = nodes_queue_list[start_node_ind]
    curr_queue_size = len(curr_queue)
    for _ in range(curr_queue_size):
        new_node_ind = curr_queue.popleft()
        for neighbour_node_ind in undir_adj_list[new_node_ind]:
            if neighbour_node_ind not in curr_calc_dist_dict:
                curr_calc_dist_dict[neighbour_node_ind] = curr_dist + 1
                curr_queue.append(neighbour_node_ind)


def updateDistList(uncalc_dist_list: List[Set[int]],
                   calc_dist_list: List[Dict[int, int]],
                   dist_list: List[int],
                   start_node_ind: int) -> int:
    """
    Returns number of elements inserted into dist_list
    """
    new_calculated_distances_list: List[int] = []
    for ind in uncalc_dist_list[start_node_ind]:
        if len(calc_dist_list[start_node_ind]) > len(calc_dist_list[ind]):
            for node, dist in calc_dist_list[ind].items():
                if node in calc_dist_list[start_node_ind]:
                    dist_list.append(
                        calc_dist_list[start_node_ind][node] + dist)
                    new_calculated_distances_list.append(ind)
                    break
        else:
            for node, dist in calc_dist_list[start_node_ind].items():
                if node in calc_dist_list[ind]:
                    dist_list.append(calc_dist_list[ind][node] + dist)
                    new_calculated_distances_list.append(ind)
                    break
    appended = len(new_calculated_distances_list)
    for ind in new_calculated_distances_list:
        uncalc_dist_list[start_node_ind].remove(ind)
        uncalc_dist_list[ind].remove(start_node_ind)
    return appended


def getDistancesListParallelBFS(undir_adj_list: Tuple[Set[int], ...],
                                index_to_node_map: List[int],
                                selected_nodes_num: int) -> List[int]:
    """
    Returns list of distances between selected nodes
    Algorithm uses parallel launch of BFS from selected vertices
    """
    uncalc_dist_list: List[Set[int]] = [set()
                                        for _ in range(selected_nodes_num)]
    for i in range(selected_nodes_num):
        for j in range(selected_nodes_num):
            if i == j:
                continue
            uncalc_dist_list[i].add(j)
    uncalc_dist_number = ((selected_nodes_num) * (selected_nodes_num - 1)) // 2
    dist_list: List[int] = []
    nodes_queue_list = [deque([index_to_node_map[i]])
                        for i in range(selected_nodes_num)]
    calc_dist_list = [{index_to_node_map[i]: 0}
                      for i in range(selected_nodes_num)]
    curr_dist = -1
    while uncalc_dist_number > 0:
        curr_dist += 1
        for start_node_ind in range(selected_nodes_num):
            if uncalc_dist_number == 0:
                return dist_list
            if len(uncalc_dist_list[start_node_ind]) > 0:
                launchNextBFSIteration(undir_adj_list, calc_dist_list,
                                       nodes_queue_list, start_node_ind,
                                       curr_dist)
                uncalc_dist_number -= updateDistList(uncalc_dist_list,
                                                     calc_dist_list,
                                                     dist_list, start_node_ind)
    return dist_list


def getNodeListWithSnowballBFS(undir_adj_list: Tuple[Set[int], ...],
                               init_adj_nodes_list: List[int],
                               max_subgraph_nodes: int) -> List[int]:
    """
    Returns list of nodes constructed by snowball subgraph method
    """
    subgraph_nodes_list: List[int] = []
    # We use a set instead of a list since the set of visited vertices
    # is limited by max_subgraph_nodes
    visited: Set[int] = set()
    nodes_queue: Deque[int] = deque()
    for init_node in init_adj_nodes_list:
        visited.add(init_node)
        nodes_queue.append(init_node)
    curr_subgraph_nodes_size = 0
    while nodes_queue and curr_subgraph_nodes_size < max_subgraph_nodes:
        new_node_ind = nodes_queue.popleft()
        subgraph_nodes_list.append(new_node_ind)
        curr_subgraph_nodes_size += 1
        for neighbour_node_ind in undir_adj_list[new_node_ind]:
            if neighbour_node_ind not in visited:
                visited.add(neighbour_node_ind)
                nodes_queue.append(neighbour_node_ind)
    return subgraph_nodes_list
