import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

class Solution {

    private List<Integer>[] graph;
    private boolean[][] visited_plus_restricted;
    private int count;
    public int reachableNodes(int n, int[][] edges, int[] restricted) {
        graph = adjencyList(edges, n);
        visited_plus_restricted = new boolean[n][2]; // {{visited, restricted}, ...edges.}
        for (int r : restricted) { // restricted nodes 
            visited_plus_restricted[r][1] = true;
        }
        bfs(0);

        return count;

    }
    public void bfs(int start) {
        Queue<Integer> queue = new LinkedList<>();
        queue.add(start);
        visited_plus_restricted[start][0] = true;
        ++count;

        while (!queue.isEmpty()) {
            int node = queue.poll();
            for (int u : graph[node]) {
                if (!visited_plus_restricted[u][0] && !visited_plus_restricted[u][1]) {
                    visited_plus_restricted[u][0] = true;
                    ++count;
                    queue.add(u);
                }
            }
        }
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

}
