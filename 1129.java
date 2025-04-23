import java.util.*;

class Solution {
    public int[] shortestAlternatingPaths(int n, int[][] redEdges, int[][] blueEdges) {
        List<Integer>[] red = new ArrayList[n];
        List<Integer>[] blue = new ArrayList[n];
        for (int i = 0; i < n; i++) {
            red[i] = new ArrayList<>();
            blue[i] = new ArrayList<>();
        }
        for (int[] e : redEdges) red[e[0]].add(e[1]);
        for (int[] e : blueEdges) blue[e[0]].add(e[1]);

        int[] ans = new int[n];
        for (int target = 0; target < n; target++) {
            ans[target] = bfs(n, target, red, blue);
        }
        return ans;
    }

    private int bfs(int n, int target, List<Integer>[] red, List<Integer>[] blue) {

        if (target == 0) return 0;

        boolean[][] seen = new boolean[n][2];
        Deque<int[]> q = new ArrayDeque<>();
        q.offer(new int[]{0, -1, 0});

        while (!q.isEmpty()) {
            int[] cur = q.poll();
            int v = cur[0];
            int col = cur[1];
            int d = cur[2];

            if (v == target) return d;

            if (col != 0) {
                for (int nxt : red[v]) {
                    if (!seen[nxt][0]) {
                        seen[nxt][0] = true;
                        q.offer(new int[]{nxt, 0, d + 1});
                    }
                }
            }
            if (col != 1) {
                for (int nxt : blue[v]) {
                    if (!seen[nxt][1]) {
                        seen[nxt][1] = true;
                        q.offer(new int[]{nxt, 1, d + 1});
                    }
                }
            }
        }
        return -1;
    }
}