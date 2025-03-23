import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Stack;

class Solution {
    public List<Integer> eventualSafeNodes(int[][] graph) {
        boolean[] visited = new boolean[graph.length];
        boolean[] safe = new boolean[graph.length];
        List<Integer> result = new ArrayList<>();
        Arrays.fill(safe, true);
        for (int i = 0; i < graph.length; ++i) {
            if (!visited[i]) {
                dfs(graph, i, visited, safe);
            }
        }

        for (int i = 0; i < graph.length; ++i) {
            if (safe[i]) {
                result.add(i);
                System.out.println(i);
            }
        }

        return result;
    }

    public void dfs(int[][] graph, int start, boolean[] visited, boolean[] safe) {
        Stack<Integer> path = new Stack<>();
        dfsHelper(graph, start, visited, path, safe);
    }

    private void dfsHelper(int[][] graph, int node, boolean[] visited, Stack<Integer> path, boolean[] safe) {
        if (visited[node]) {
            return;
        }
        visited[node] = true;
        path.push(node);

        for (int u : graph[node]) {
            if (!visited[u]) {
                dfsHelper(graph, u, visited, path, safe);
            } else if ((path.contains(u) || Arrays.binarySearch(graph[u], u) >= 0 || safe[u]==false)) //есть ли цикл или ведет в вершину у которой есть ребро в себя) 
            {
                for (int p : path) {
                    safe[p] = false;
                }

            }
        }

        path.pop();
    }

    public static void main(String[] args) {
        Solution solution = new Solution();
        int[][] graph = {{1, 2}, {2, 3}, {5}, {0}, {5}, {}, {}};
        List<Integer> safeNodes = solution.eventualSafeNodes(graph);

        System.out.println("Safe nodes: " + safeNodes);
    }
}
