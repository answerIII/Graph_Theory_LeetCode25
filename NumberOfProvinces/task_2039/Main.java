package org.example;

import java.util.*;

class Solution {
    public int networkBecomesIdle(int[][] edges, int[] patience) {
        List<List<Integer>> graph = new ArrayList<>();
        List<Integer> lengths = new ArrayList<>(patience.length);
        for (int i = 0; i < patience.length; ++i) {
            lengths.add(0);
            graph.add(new ArrayList<>());
        }
        for (int[] edge : edges) {
            graph.get(edge[0]).add(edge[1]);
            graph.get(edge[1]).add(edge[0]);
        }

        Queue<Integer> queue = new LinkedList<>();
        queue.add(0);
        while (!queue.isEmpty()) {
            int node = queue.poll();
            for (int neighbor : graph.get(node)) {
                if (lengths.get(node) == 0){
                    lengths.set(neighbor,lengths.get(node)+1);
                }
            }
        }

        int maxTime = 0;
        for (int i = 0; i < patience.length; ++i) {
            int curr = 2 * lengths.get(i);
            if (curr - patience[i] > 0) {curr+=curr - patience[i];}
            maxTime = Math.max(maxTime,curr);
        }
        return maxTime;
    }
}

public class Main {

    public static void main(String[] args) {
        Solution s=new Solution();
    }

}