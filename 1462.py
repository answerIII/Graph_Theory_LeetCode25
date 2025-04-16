from typing import List
from collections import defaultdict

class Solution:
    def DFS(self, course, visited, component, course_map):
        visited[course] = 1
        component.append(course)
        
        for next_course in course_map[course]:
            if visited[next_course] == 0: 
                self.DFS(next_course, visited, component, course_map)

    def checkIfPrerequisite(self, numCourses: int, prerequisites: List[List[int]], queries: List[List[int]]) -> List[bool]:
        course_map = defaultdict(list)
        for course, prereq in prerequisites:
            course_map[course].append(prereq)
        
        visited = [0] * numCourses  # 0- не посещали, 1- посещена
        components = []
        course_to_component = {} 

        for course in range(numCourses):
            if visited[course] == 0:
                component = []
                self.DFS(course, visited, component, course_map)
                components.append(component)  

                for c in component:
                    course_to_component[c] = len(components) - 1
        
        answer = []
        for uj, vj in queries:
            uj_component_idx=course_to_component[uj]
            vj_component_idx=course_to_component[vj]
            if  uj_component_idx != vj_component_idx:
                answer.append(False)
            else:
                answer.append(components[uj_component_idx].index(uj) < components[vj_component_idx].index(vj))
        
        return answer
