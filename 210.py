from typing import List

class Solution:
    def findOrder(self, numCourses: int, prerequisites: List[List[int]]) -> List[int]:
        
        def DFS(course):
            if visited[course] == 0:
                visited[course] = 1
                
                for next_course in course_map[course]:
                    if DFC(next_course) == False:
                        return False
                        
                res.append(course)
                visited[course] = 2
                return True
                
            if visited[course] == 1: #нашли цикл
                return False
                
            if visited[course] == 2:
                return True
                
        course_map = [[] for _ in range(numCourses)]
        for course, prereq in prerequisites:
            course_map[prereq].append(course)
            
        visited = [0]* numCourses  # 0-не трогали, 2-идем по цепочке, 3-уже прошли
        res = []

        for course in range(numCourses):
            if visited[course] == 0:
                if DFS(course) == False:
                    return []

        return res.reverse()