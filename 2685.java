import java.util.ArrayList;
import java.util.List;

class Solution {

    static public int countCompleteComponents(int n, int[][] edges) {
        int number_of_visited = 0;
        int number_of_components = 0;
        List<Integer>[] graph = adjencyList(edges, n);

        boolean[] visited = new boolean[n];
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                List<Integer> component = new ArrayList<>();
                dfs(graph, i, visited, component);
                int complete_component = 1;
                for (Integer vert : component) {
                    if (graph[vert].size() != (component.size() - 1)) {
                        complete_component = 0;
                        break;
                    }
                }
                number_of_components += complete_component;
            }
        }
        return number_of_components;
    }

    static List<Integer>[] adjencyList(int[][] edges, int n) {
        List<Integer>[] adj_list = new ArrayList[n];
        for (int i = 0; i < n; i++) {
            adj_list[i] = new ArrayList<>();
        }
        for (int[] edge : edges) {
            adj_list[edge[0]].add(edge[1]);
            adj_list[edge[1]].add(edge[0]);
        }
        return adj_list;

    }

    static void dfs(List<Integer>[] graph, int node, boolean[] visited, List<Integer> component) {
        if (visited[node]) {
            return;
        }
        visited[node] = true;
        component.add(node);
        for (int v : graph[node]) {
            if (!visited[v]) {
                dfs(graph, v, visited, component);
            }
        }
    }

}
