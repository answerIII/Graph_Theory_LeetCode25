def dfs(node, graph, visited):
    if node in visited:
        return False
    visited.add(node)
    for child in graph[node]:
        if child != -1:
            if not dfs(child, graph, visited):
                return False
    return True

class Solution(object):
    def validateBinaryTreeNodes(self, n, leftChild, rightChild):
        """
        :type n: int
        :type leftChild: List[int]
        :type rightChild: List[int]
        :rtype: bool
        """
        parents = [0] * n
        for i in range(n):
            if leftChild[i] != -1:
                parents[leftChild[i]] += 1
                if parents[leftChild[i]] > 1:
                    return False
            if rightChild[i] != -1:
                parents[rightChild[i]] += 1
                if parents[rightChild[i]] > 1:
                    return False
                    
        roots = [] 
        for i in range(n):
            if parents[i] == 0:
                roots.append(i)
        if len(roots) != 1 or len(roots) == 0:
            return False

        graph = {i: [] for i in range(n)}
        for i in range(n):
            graph[i].append(leftChild[i])
            graph[i].append(rightChild[i])

        visited = set()
        print(graph)
        if not dfs(roots[0], graph, visited):
            return False

        return len(visited) == n