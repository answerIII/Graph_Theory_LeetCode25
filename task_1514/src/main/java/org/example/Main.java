package org.example;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Stack;
import java.util.function.ToIntFunction;

class Solution {
  class EdgeWithProb {
    int[] edge;
    double prob;

    public EdgeWithProb(int[] edge, double prob) {
      this.edge = edge;
      this.prob = prob;
    }
  }
  public double maxProbability(int n, int[][] edges, double[] succProb, int start_node, int end_node) {
    EdgeWithProb[] combined = new EdgeWithProb[edges.length];
    for (int i = 0; i < edges.length; i++) {
      combined[i] = new EdgeWithProb(edges[i], succProb[i]);
    }
    Arrays.sort(combined, Comparator.comparingInt(edgeWithProb -> edgeWithProb.edge[0])); // e log e

    Stack<Integer> stack = new Stack<>();
    stack.push(start_node);
    while(!stack.empty()){
      int node = stack.pop();
      int index = binarySearch(combined, node);
    }
    return 0;
  }

  public static int binarySearch(EdgeWithProb[] arr, int target) {
    int left = 0;
    int right = arr.length - 1;
    while (left <= right) {
      int mid = left + (right - left) / 2;
      if (arr[mid].edge[0] == target) {
        return mid;
      } else if (arr[mid].edge[0] < target) {
        left = mid + 1;
      } else {
        right = mid - 1;
      }
    }
    return -1;
  }
  void findMinPath(List<List<Integer>> lists, int start_node, int end_node, double[] succProb, int[] pathLengths){
    for(Integer indexNode : lists.get(start_node)){
      if(pathLengths[indexNode] > pathLengths[start_node])
    }
  }
}

public class Main {

  public static void main(String[] args) {
    System.out.println("Hello world!");
  }

}