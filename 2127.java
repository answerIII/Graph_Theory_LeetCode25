import java.util.*;

class Solution {
    public int maximumInvitations(int[] favorite) {
        int n = favorite.length;
        int[] indeg = new int[n];
        for (int v : favorite) indeg[v]++;

        int[] depth = new int[n];
        Deque<Integer> q = new ArrayDeque<>();

        for (int i = 0; i < n; ++i)
            if (indeg[i] == 0) q.offer(i);

        while (!q.isEmpty()) {
            int u = q.poll();
            int v = favorite[u];
            depth[v] = Math.max(depth[v], depth[u] + 1);
            if (--indeg[v] == 0) q.offer(v);
        }

        boolean[] seen = new boolean[n];
        int maxCycle = 0;
        for (int i = 0; i < n; ++i) {
            if (indeg[i] > 0 && !seen[i]) {
                int len = 0, cur = i;
                while (!seen[cur]) {
                    seen[cur] = true;
                    cur = favorite[cur];
                    len++;
                }
                maxCycle = Math.max(maxCycle, len);
            }
        }

        int pairSum = 0;
        for (int a = 0; a < n; ++a) {
            int b = favorite[a];
            if (a < b && favorite[b] == a) { 
                pairSum += 2 + depth[a] + depth[b];
            }
        }

        return Math.max(maxCycle, pairSum);
    }
}