package org.example;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

class Solution {
  public int[] gardenNoAdj(int n, int[][] paths) {
    int[] flowers = new int[n];
    List<List<Integer>> lists = new ArrayList<>(n);
    for (int i = 0; i < n; i++) {
      lists.add(new ArrayList<>());
    }
    for(int[] path : paths){
      lists.get(path[0]-1).add(path[1]-1);
      lists.get(path[1]-1).add(path[0]-1);
    }
    for(int i = 0; i < n; ++i){
      boolean[] colors = new boolean[5];
      for(Integer friend : lists.get(i)){
        if(flowers[friend]!= 0){
          colors[flowers[friend]] = true;
        }
      }
      for(int j =1; j<5; ++j){
        if(!colors[j]){
          flowers[i] = j;
        }
      }
    }
    return flowers;
  }
}
public class Main {

  public static void main(String[] args) {
    System.out.println("Hello world!");
  }

}