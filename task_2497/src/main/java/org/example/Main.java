package org.example;

import java.util.Arrays;
import java.util.Comparator;

class Solution {
  public int maxStarSum(int[] vals, int[][] edges, int k) {
    int max = Integer.MIN_VALUE;
    Arrays.sort(edges, Comparator.comparingInt(a -> a[0]));
    for(int i = 0; i < edges.length; ++i) {
      int[] potential = new int[k];
      int t = 0;
      int cur = edges[i][0];
      if(t<k && vals[edges[i][1]] > 0){
        potential[t++] = vals[edges[i][1]];
      }
      while(i+1 != edges.length && edges[i][0] == edges[i+1][0]){
        if(t < k) break;
        if(vals[edges[i][1]] > 0){
          potential[t++] = vals[edges[i][1]];
        }
      }
      Arrays.sort(potential, 0, t);
      --t;
      while(t != -1){
        cur += potential[t--];
      }
      if(cur > max) max = cur;
    }
    return max;
  }
}
public class Main {

  public static void main(String[] args) {
    Solution solution = new Solution();
    int[] vals = {1,2,3,4,10,-10,-20};
    int[][] edges = {{0,1},{1,2},{1,3},{3,4},{3,5},{3,6}};
    int k = 2;
    System.out.println(solution.maxStarSum(vals, edges, k));
  }

}