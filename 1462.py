from typing import List
from collections import defaultdict

class Solution:
    def DFS(self, course, visited, graph):
        visited[course] = 1
        for next_course in graph[course]:
            if visited[next_course] == 0:
                self.DFS(next_course, visited, graph)

    def checkIfPrerequisite(self, numCourses: int, prerequisites: List[List[int]], queries: List[List[int]]) -> List[bool]:
        graph = defaultdict(list)
        for prereq, course in prerequisites:
            graph[prereq].append(course)

        reachable = defaultdict(set)

        for course in range(numCourses):
            visited = [0] * numCourses
            self.DFS(course, visited, graph)
            for i in range(numCourses):
                if visited[i]:
                    reachable[course].add(i)

        answer = []
        for uj, vj in queries:
            answer.append(vj in reachable[uj])
        
        return answer
