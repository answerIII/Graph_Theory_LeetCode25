package org.example;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.PriorityQueue;
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
    double[] probabilities = new double[n];
    probabilities[start_node] = 1;
    PriorityQueue<double[]> queue = new PriorityQueue<>(Comparator.comparingDouble(doubles -> doubles[1]));
    boolean[] alreadyChecked = new boolean[n];
    queue.add(new double[]{start_node, 1.0});


    while(!queue.isEmpty()){
      double[] parent = queue.poll();
      int parentIndex = (int) parent[0];
      alreadyChecked[parentIndex] = true;
      if(parentIndex == end_node) {
        return parent[1];
      }
      for(double[] friend : graph.get(parentIndex)){
        int friendIndex = (int)friend[0];
        if(alreadyChecked[friendIndex]){
          continue;
        }
        double way = probabilities[parentIndex] * parent[1];
        if(way > probabilities[friendIndex]){
          probabilities[friendIndex] = way;
          friend[1] = way;
        }
      }
    }
    return -1;
  }
}

public class Main {

  public static void main(String[] args) {
    System.out.println("Hello world!");
  }

}