package org.example;

import org.w3c.dom.Node;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class Solution {
  public List<Integer> findSmallestSetOfVertices(int n, List<List<Integer>> edges) {
    int[] nodes = new int[n];
    for(List<Integer> edge: edges){
      ++nodes[edge.get(1)];
    }
    List<Integer> list = new ArrayList<>();
    for(int i=0;i<n; ++i){
      if(nodes[i] == 0) list.add(i);
    }
    return list;
  }
}

public class Main {

  public static void main(String[] args) {
    Solution s=new Solution();
    int n = 6;
    List<List<Integer>> edges = new ArrayList<>();
    edges.add(Arrays.asList(0, 1));
    edges.add(Arrays.asList(0, 2));
    edges.add(Arrays.asList(2, 5));
    edges.add(Arrays.asList(3, 4));
    edges.add(Arrays.asList(4, 2));

    System.out.println(s.findSmallestSetOfVertices(n,edges));
  }

}