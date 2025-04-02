import java.util.*;

class Solution {

    private List<int[]>[] graph;
    private int n;

    public boolean prim(int start_vertex, int end_vertex, int limit) {
        PriorityQueue<int[]> queue = new PriorityQueue<>(Comparator.comparingInt(a -> a[1]));
        boolean[] visited_vertex = new boolean[n];
        int[][] edges_of_prim = new int[n][2];
        int edgesUsed = 0;

        queue.add(new int[] { start_vertex, 0 });

        while (!queue.isEmpty() && edgesUsed < n) {
            int[] curr = queue.poll();
            int node = curr[0];
            int weight = curr[1];
            if (visited_vertex[node]) {
                continue;
            }
            visited_vertex[node] = true;
            edges_of_prim[edgesUsed++] = new int[] { node, weight };
            if (weight >= limit) {
                return false;
            }
            if (node == end_vertex) {
                return true;
            }
            for (int[] neighbor : graph[node]) {
                if (!visited_vertex[neighbor[0]]) {
                    queue.add(new int[] { neighbor[0], neighbor[1] });
                }
            }
        }
        return false;
    }

    static List<int[]>[] adjencyListWithWeights(int[][] edges, int n) {
        List<int[]>[] adj_list = new ArrayList[n];
        for (int i = 0; i < n; i++) {
            adj_list[i] = new ArrayList<>();
        }
        for (int[] edge : edges) {
            int p = edge[0], q = edge[1], limit = edge[2];
            adj_list[p].add(new int[] { q, limit });
            adj_list[q].add(new int[] { p, limit });
        }
        return adj_list;

    }

    public boolean[] distanceLimitedPathsExist(int n, int[][] edgeList, int[][] queries) {
        graph = adjencyListWithWeights(edgeList, n);

        this.n = n;
        boolean[] result = new boolean[queries.length];

        for (int i = 0; i < queries.length; ++i) {
            result[i] = prim(queries[i][0], queries[i][1], queries[i][2]);
        }
        return result;

    }
}