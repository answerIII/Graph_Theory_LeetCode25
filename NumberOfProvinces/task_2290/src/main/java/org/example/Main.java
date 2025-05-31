package org.example;

import java.util.ArrayDeque;
import java.util.Arrays;
import java.util.Deque;

class Solution {
  public int minimumObstacles(final int[][] grid) {
    int n = grid.length;
    int m = grid[0].length;
    int[][] cellsList = new int[n][];
    for (int i = 0; i < grid.length; ++i) {
      cellsList[i] = grid[i].clone();
    }
    int[][] directions = new int[][]{{1,0},{-1,0},{0,1},{0,-1}};
    for (int[] cells : cellsList) {
      Arrays.fill(cells, Integer.MAX_VALUE);
    }
    cellsList[0][0] = grid[0][0];
    Deque<int[]> deque = new ArrayDeque<>();
    deque.add(new int[]{0,0});

    while(!deque.isEmpty()){
      int[] cell =  deque.pollFirst();
      int i = cell[0];
      int j = cell[1];
      if(i == n-1 && j == m-1){
        return cellsList[i][j];
      }
      for(int[] direction : directions){
        i+=direction[0];
        j+=direction[1];
        if(i < n && j < m && i > -1 && j > -1){
          int way = cellsList[cell[0]][cell[1]] + grid[i][j];
          if(way < cellsList[i][j]){
            cellsList[i][j] = way;
            if(grid[i][j]==1) {
              deque.addLast(new int[]{i, j});
            }
            else{
              deque.addFirst(new int[]{i, j});
            }
          }
        }
        i = cell[0];
        j = cell[1];
      }
    }
    throw new RuntimeException("program error");
  }
}

public class Main {

  public static void main(String[] args) {
    Solution s = new Solution();
    int[][] grid = {
            {0, 1, 0, 0, 0},
            {0, 1, 0, 1, 0},
            {0, 0, 0, 1, 0}
    };
    System.out.println(s.minimumObstacles(grid));
  }

}