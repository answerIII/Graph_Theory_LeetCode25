from typing import List
from collections import defaultdict

class Solution:
    def DFS(self, course, visited, res, course_map):
        if visited[course] == 0:
            visited[course] = 1
            
            for next_course in course_map[course]:
                if self.DFS(next_course, visited, res, course_map) == False:
                    return False
                    
            res.append(course)
            visited[course] = 2
            return True
            
        if visited[course] == 1: #нашли цикл
            return False
            
        if visited[course] == 2:
            return True

    def checkIfPrerequisite(self, numCourses: int, prerequisites: List[List[int]], queries: List[List[int]]) -> List[bool]:
        course_map = defaultdict(list)
        for course, prereq in prerequisites:
            course_map[course].append(prereq)  

        visited = [0] * numCourses  # 0-не трогали, 2-идем по цепочке, 3-уже прошли
        res = []

        for course in range(numCourses):
            if visited[course] == 0:
                if self.DFS(course, visited, res, course_map) == False:
                    return []
        res.reverse()

        course_to_index = {course: idx for idx, course in enumerate(res)}
        answer = []
        for uj, vj in queries:
            answer.append(course_to_index[uj] < course_to_index[vj])

        return answer