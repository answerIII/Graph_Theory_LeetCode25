package org.example;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class Solution {
  class Node{
    List<Node> parents = new ArrayList<>();
    boolean isFound;
    int number;
    Node(int number){
      this.number = number;
    }

    public void findParent(List<Integer> vertexes, boolean byCycle) {
      if(isFound && parents.size() == 0 && !byCycle) {
        vertexes.set(number, -1);
        return;
      }
      else if(parents.size() == 0){
        vertexes.set(number, number);
        return;
      }
      if (isFound) return;
      isFound = true;
      for(Node parent: parents){
        parent.findParent(vertexes, false);
      }
    }

  }
  public List<Integer> findSmallestSetOfVertices(int n, List<List<Integer>> edges) {
    Node[] nodes = new Node[n];
    List<Integer> vertexes = new ArrayList<>(n);
    for (int i=0; i< n ; ++i) {
      vertexes.add(-1);
    }
    for (int i=0; i < n; ++i) {
      nodes[i] = new Node(i);
    }
    for(List<Integer> edge: edges){
      nodes[edge.get(1)].parents.add(nodes[edge.get(0)]);
    }
    for (Node node : nodes) {
      if (!node.isFound) node.findParent(vertexes, true);
    }
    List<Integer> minVertexes = new ArrayList<>();
    for (Integer vertex : vertexes) {
      if(vertex != -1) minVertexes.add(vertex);
    }
    return minVertexes;
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