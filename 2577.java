import java.util.*;

class Solution {
    public int minimumTime(int[][] grid) {
        int m = grid.length, n = grid[0].length;
        if (m == 1 && n == 1) return 0;

        if (m > 1 && n > 1 && grid[0][1] > 1 && grid[1][0] > 1) return -1;

        int[][] dist = new int[m][n];
        for (int[] row : dist) Arrays.fill(row, Integer.MAX_VALUE / 2);

        PriorityQueue<int[]> pq = new PriorityQueue<>(Comparator.comparingInt(a -> a[0]));
        dist[0][0] = 0;
        pq.offer(new int[]{0, 0, 0});

        int[][] dirs = {{1,0}, {-1,0}, {0,1}, {0,-1}};

        while (!pq.isEmpty()) {
            int[] cur = pq.poll();
            int t = cur[0], r = cur[1], c = cur[2];
            if (t != dist[r][c]) continue;

            if (r == m - 1 && c == n - 1)
                return t;

            for (int[] d : dirs) {
                int nr = r + d[0], nc = c + d[1];
                if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;

                int nt = t + 1;
                int g  = grid[nr][nc];
                if (nt < g) {
                    int diff = g - nt;
                    if ((diff & 1) == 1) g++;
                    nt = g;
                }

                if (nt < dist[nr][nc]) {
                    dist[nr][nc] = nt;
                    pq.offer(new int[]{nt, nr, nc});
                }
            }
        }
        return -1; 
    }
}