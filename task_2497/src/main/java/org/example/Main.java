package org.example;

import java.util.Arrays;

class Solution {
  public int maxStarSum(int[] vals, int[][] edges, int k) {
    int max = Integer.MIN_VALUE;
    for(int i = 0; i < edges.length; ++i) {
      int[] potential = new int[k];
      int t = 0;
      int cur = edges[i][i];
      for(int j = 0; j < edges.length; ++j) {
        if(edges[i][j] == 1 && i!=j && vals[j] > 0) {
          potential[t++] = vals[j];
        }
      }
      Arrays.sort(potential, 0, k);
      t = 0;
      while(t != k){
        cur += potential[t++];
      }
      if(cur > max) max = cur;
    }
    return max;
  }
}
public class Main {

  public static void main(String[] args) {
    Solution s = new Solution();
  }

}