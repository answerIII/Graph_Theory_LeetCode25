import java.util.*;

class Solution {
    public int minCostConnectPoints(int[][] points) {
        int n = points.length;
        PriorityQueue<int[]> pq = new PriorityQueue<>(Comparator.comparingInt(a -> a[0]));
        boolean[] visited = new boolean[n];
        int result = 0;
        int edgesUsed = 0;
        
        pq.offer(new int[]{0, 0});
        
        while (edgesUsed < n) {
            int[] curr = pq.poll();
            int cost = curr[0];
            int point = curr[1];
            
            if (visited[point]) continue;
            
            result += cost;
            visited[point] = true;
            edgesUsed++;
            
            for (int i = 0; i < n; ++i) {
                if (!visited[i]) {
                    int newCost = Math.abs(points[point][0] - points[i][0]) + Math.abs(points[point][1] - points[i][1]);
                    pq.offer(new int[]{newCost, i});
                }
            }
        }
        
        return result;
    }
}