import java.util.*;

class Solution {
    public int minMaxWeight(int n, int[][] edges, int threshold) {
        if (threshold <= 0) {
            return n == 1 ? 0 : -1;
        }
        
        List<List<int[]>> adj = new ArrayList<>(n);
        for (int i = 0; i < n; i++) {
            adj.add(new ArrayList<>());
        }
        for (int[] e : edges) {
            int u = e[0], v = e[1], w = e[2];
            adj.get(v).add(new int[]{u, w});
        }

        int[] dist = new int[n];
        Arrays.fill(dist, Integer.MAX_VALUE);
        dist[0] = 0;
        
        PriorityQueue<int[]> pq = new PriorityQueue<>(
            (a, b) -> Integer.compare(a[0], b[0])
        );
        pq.offer(new int[]{0, 0});
        
        while (!pq.isEmpty()) {
            int[] top = pq.poll();
            int cost = top[0], u = top[1];
            if (cost > dist[u]) continue;
            
            for (int[] nxt : adj.get(u)) {
                int v = nxt[0], w = nxt[1];
                int nc = Math.max(cost, w);
                if (nc < dist[v]) {
                    dist[v] = nc;
                    pq.offer(new int[]{nc, v});
                }
            }
        }
        
        int answer = 0;
        for (int i = 0; i < n; i++) {
            if (dist[i] == Integer.MAX_VALUE) {
                return -1;
            }
            answer = Math.max(answer, dist[i]);
        }
        
        return answer;
    }
}