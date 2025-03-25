
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class Solution {

    private List<Integer>[] graph;
    private boolean[] visited;
    private int count;

    public int minReorder(int n, int[][] connections) {
            count = 0;
            graph = adjencyListAndInvert(connections, n);
            visited = new boolean[n];
            count = 0;
            
            dfs(0);
            return count;

}
    static List<Integer>[] adjencyListAndInvert(int[][] edges, int n) {
        List<Integer>[] adj_list = new ArrayList[n];
        for (int i = 0; i < n; i++) {
            adj_list[i] = new ArrayList<>();
        }
        for (int[] edge : edges) {
            //inverting
            adj_list[edge[1]].add(edge[0]);
            adj_list[edge[0]].add(-edge[1]);

        }
        return adj_list;

    }

    void dfs(int node) {
        if (visited[node]) {
            return;
        }
        visited[node] = true;
        for (int u : graph[node]) {
                if (u < 0 && !visited[-u]) {
                    ++count;
                    dfs(-u);

                } else if (u>0 && !visited[u]) {
                    dfs(u);
                }
            
            }
    }

}
