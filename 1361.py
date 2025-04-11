class Solution(object):
    def validateBinaryTreeNodes(self, n, leftChild, rightChild):
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

        visited = [False] * n
        stack = [roots[0]]

        while stack:
            node = stack.pop()
            if visited[node]:
                return False
            visited[node] = True
            if leftChild[node] != -1:
                stack.append(leftChild[node])
            if rightChild[node] != -1:
                stack.append(rightChild[node])

        return all(visited)
