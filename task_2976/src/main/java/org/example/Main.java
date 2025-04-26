package org.example;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.PriorityQueue;

class Solution {
  static final int OFFSET = 97;
  public long minimumCost(String source, String target, char[] original, char[] changed, int[] cost) {
    int[][] vocabulary = new int[26][26];
    long totalCost =0;
    List<List<int[]>> paths = new ArrayList<>(original.length);
    for (int i =0; i < 28; ++i) {
      paths.add(new ArrayList<>());
    }
    for(int i = 0; i < original.length; ++i){
      paths.get((int)original[i] - OFFSET)
              .add(new int[] {(int)changed[i] - OFFSET, cost[i]});
    }
    pullMostCheapest(paths,vocabulary);
    for(int i = 0; i < source.length(); ++i){
      int sourceS = (int) source.charAt(i) - OFFSET;
      int targetS = (int) target.charAt(i) - OFFSET;
      if(source.charAt(i)!=target.charAt(i) && vocabulary[sourceS][targetS] == -1) {
        return -1;
      }
      totalCost+=vocabulary[sourceS][targetS];
    }
    return totalCost;
  }

  private void pullMostCheapest(List<List<int[]>> paths, int[][] vocabulary) {
    for(int i=0; i < paths.size(); ++i){
      Arrays.fill(vocabulary[i], Integer.MAX_VALUE);
      vocabulary[i][i] = 0;
    }
    for(int i=0; i < 26; ++i){
      PriorityQueue<int[]> queue = new PriorityQueue<>(Comparator.comparingInt(a -> a[1]));
      queue.add(new int[]{i, 0});
      while(!queue.isEmpty()){
        int[] parent = queue.poll();
        if(parent[1] > vocabulary[i][parent[0]]) {
          continue;
        }
        for(int[] friend : paths.get(parent[0])){
          int friendIndex = friend[0];
          int way = friend[1] + parent[1];
          if(way < vocabulary[i][friendIndex]){
            vocabulary[i][friendIndex] = way;
            queue.add(new int[]{friendIndex, way});
          }
        }
      }
    }
  }

}

public class Main {



  public static void main(String[] args) {
    Solution s = new Solution();
    String source = "aaaa";
    String target = "bbbb";
    char[] original = {'a', 'c'};
    char[] changed = {'b', 'b'};
    int[] cost = {1, 2};

    int result = (int) s.minimumCost(source, target, original, changed, cost);
    System.out.println("Minimum conversion cost: " + result);
  }

}