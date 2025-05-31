package org.example;

import java.util.*;

class Solution {
    public int networkBecomesIdle(int[][] edges, int[] patience) {
        List<List<Integer>> graph = new ArrayList<>();
        List<Integer> lengths = new ArrayList<>(patience.length);
        for (int i = 0; i < patience.length; ++i) {
            lengths.add(-1);
            graph.add(new ArrayList<>());
        }
        lengths.set(0,0);
        for (int[] edge : edges) {
            graph.get(edge[0]).add(edge[1]);
            graph.get(edge[1]).add(edge[0]);
        }

        Queue<Integer> queue = new LinkedList<>();
        queue.add(0);
        while (!queue.isEmpty()) {
            int node = queue.poll();
            for (int neighbor : graph.get(node)) {
                if (lengths.get(neighbor) == -1){
                    lengths.set(neighbor,lengths.get(node)+1);
                    queue.add(neighbor);
                }
            }
        }

        int maxTime = 0;
        for (int i = 0; i < patience.length; ++i) {
            int curr = 2 * lengths.get(i);
            int lastSent = patience[i] * ((curr -1)/patience[i]);
            maxTime = Math.max(maxTime,curr+lastSent);
        }
        return maxTime + 1;
    }
}

public class Main {

    public static void main(String[] args) {
        Solution s=new Solution();
    }

}