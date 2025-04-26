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

    public void findParent(List<Integer> vertexes) {
      if(parents.size() == 0 && isFound){
        vertexes.set(number,-1);
      }
      if(isFound) return;
      isFound = true;
      for(Node parent: parents){
        parent.findParent(vertexes);
      }
    }

  }
  public List<Integer> findSmallestSetOfVertices(int n, List<List<Integer>> edges) {
    Node[] nodes = new Node[n];
    List<Integer> vertexes = new ArrayList<>();
    for (int i=0; i < n; ++i) {
      nodes[i] = new Node(i);
    }
    for(List<Integer> edge: edges){
      nodes[edge.get(1)].parents.add(nodes[edge.get(0)]);
    }
    for (Node node : nodes) {
      if (!node.isFound) node.findParent(vertexes);
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
    System.out.println("Hello world!");
  }

}