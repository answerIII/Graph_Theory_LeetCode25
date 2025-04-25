from typing import List
from collections import defaultdict
import heapq
import math

class Solution:
    def minimumTime(self, n: int, edges: List[List[int]], disappear: List[int]) -> List[int]:
        graph = defaultdict(list)
        for u, v, length in edges:
            graph[u].append((v, length))
            graph[v].append((u, length))
       
        visited = [False] * n
        min_times = [math.inf] * n
        min_times[0] = 0
        priority_queue = [(0, 0)]

        while priority_queue != []:
            current_time, current_node = heapq.heappop(priority_queue)

            if visited[current_node] == True:
                continue

            visited[current_node] = True

            for neighbor, edge_length in graph[current_node]:
                if visited[neighbor] == True:
                    continue

                new_time = current_time + edge_length
                if new_time <= disappear[neighbor] and new_time < min_times[neighbor]:
                    min_times[neighbor] = new_time
                    heapq.heappush(priority_queue, (new_time, neighbor))

        result = []
        for time in min_times:
            result.append(time)

        return result

