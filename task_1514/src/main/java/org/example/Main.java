package org.example;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Stack;
import java.util.function.ToIntFunction;

class Solution {
  public double maxProbability(int n, int[][] edges, double[] succProb, int start_node, int end_node) {
    List<List<double[]>> graph = new ArrayList<>();
    for (int i = 0; i < n; i++) {
      graph.add(new ArrayList<>());
    }
    for (int i = 0; i < edges.length; i++) {
      int a = edges[i][0];
      int b = edges[i][1];
      double prob = succProb[i];
      graph.get(a).add(new double[]{b, prob});
      graph.get(b).add(new double[]{a, prob});
    }
    Stack<Integer> stack = new Stack<>();
    int[] probabilities = new int[n];
    probabilities[start_node] = 1;
    stack.push(start_node);
    while(!stack.empty()){
      int parent = stack.pop();
      for(double[] friends : graph.get(parent)){
        probabilities[(int)friends[0]]
      }
    }
  }
}

public class Main {

  public static void main(String[] args) {
    System.out.println("Hello world!");
  }

}