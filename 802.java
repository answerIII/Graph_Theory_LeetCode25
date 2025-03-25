
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Stack;

class Solution {

    boolean[] visited;
    boolean[] safe;

    public List<Integer> eventualSafeNodes(int[][] graph) {
        visited = new boolean[graph.length];
        safe = new boolean[graph.length];
        List<Integer> result = new ArrayList<>();
        Arrays.fill(safe, true);
        for (int i = 0; i < graph.length; ++i) {
            if (!visited[i]) {
                dfs(graph, i);
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

    public void dfs(int[][] graph, int start) {
        Stack<Integer> path = new Stack<>();
        dfsHelper(graph, start, path);
    }

    private void dfsHelper(int[][] graph, int node, Stack<Integer> path) {
        if (visited[node]) {
            return;
        }
        visited[node] = true;
        path.push(node);

        for (int u : graph[node]) {
            if (!visited[u]) {
                dfsHelper(graph, u, path);
            } else if ((path.contains(u) || Arrays.binarySearch(graph[u], u) >= 0 || safe[u] == false)) //есть ли цикл или ведет в вершину у которой есть ребро в себя) 
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
