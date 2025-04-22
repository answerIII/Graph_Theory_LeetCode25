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
      if(edge[1] > 0){
        map.get(edge[0]).add(edge[1]);
      }
      if(edge[0] > 0){
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
    int[] vals = {-5};
    int[][] edges = {};
    int k = 0;
    System.out.println(solution.maxStarSum(vals, edges, k));
  }

}