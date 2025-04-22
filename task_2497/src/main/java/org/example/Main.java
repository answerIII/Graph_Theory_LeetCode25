package org.example;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;

class Solution {
  public int maxStarSum(int[] vals, int[][] edges, int k) {
    int max = Integer.MIN_VALUE;
    for(int val : vals){
      if(val > max) max = val;
    }
    Map<Integer, ArrayList<Integer>> map = new HashMap<>();
    for(int[] edge : edges){
      map.putIfAbsent(edge[0], new ArrayList<>());
      map.putIfAbsent(edge[1], new ArrayList<>());
      if(vals[edge[1]] > 0){
        map.get(edge[0]).add(edge[1]);
      }
      if(vals[edge[0]] > 0){
        map.get(edge[1]).add(edge[0]);
      }
    }
    for(Map.Entry<Integer, ArrayList<Integer>> entry : map.entrySet()){
      entry.getValue().sort(Comparator.comparingInt(a -> vals[(Integer) a]).reversed());
      int i = 0;
      int cur = vals[entry.getKey()];
      while(i < entry.getValue().size() && i != k){
        cur += vals[entry.getValue().get(i++)];
      }
      if (cur > max) max = cur;
    }
    return max;
  }
}
public class Main {

  public static void main(String[] args) {
    Solution solution = new Solution();
    int[] vals = {17, 49, -34, -17, -7, -23, 24};
    int[][] edges = {{3,1},{5,1},{0,3},{4,6},{1,4},{3,4},{6,3},{2,6},{5,2},{1,6},{6,0},{2,3},{3,5},{2,1},{0,2},{5,0},{0,4}};
    int k = 6;
    System.out.println(solution.maxStarSum(vals, edges, k));
  }

}