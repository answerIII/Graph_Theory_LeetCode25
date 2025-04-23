import java.util.*;

class Solution {
    private static final int RED = 0, BLUE = 1, INF = 1_000_000_000;

    public int[] shortestAlternatingPaths(int n, int[][] redEdges, int[][] blueEdges) {

        List<Integer>[] g = new ArrayList[n];
        List<Integer>[] red = new ArrayList[n], blue = new ArrayList[n];
        for (int i = 0; i < n; i++) {
            red[i] = new ArrayList<>();
            blue[i] = new ArrayList<>();
        }
        for (int[] e : redEdges)  red[e[0]].add(e[1]);
        for (int[] e : blueEdges) blue[e[0]].add(e[1]);

        int[][] dist = new int[n][2];
        for (int[] d : dist) Arrays.fill(d, INF);
        dist[0][RED] = dist[0][BLUE] = 0;

        Deque<int[]> q = new ArrayDeque<>();
        q.offer(new int[]{0, RED});
        q.offer(new int[]{0, BLUE});

        while (!q.isEmpty()) {
            int[] cur = q.poll();
            int v = cur[0];
            int col = cur[1];
            int d = dist[v][col];

            List<Integer> nextList = (col == RED) ? blue[v] : red[v];
            int nextCol = col ^ 1;

            for (int nxt : nextList) {
                if (dist[nxt][nextCol] == INF) { 
                    dist[nxt][nextCol] = d + 1;
                    q.offer(new int[]{nxt, nextCol});
                }
            }
        }

        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            int best = Math.min(dist[i][RED], dist[i][BLUE]);
            ans[i] = (best == INF) ? -1 : best;
        }
        return ans;
    }
}