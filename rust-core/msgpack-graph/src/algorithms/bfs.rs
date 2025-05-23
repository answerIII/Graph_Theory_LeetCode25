use crate::graph::Graph;
use std::collections::{HashMap, VecDeque};

pub fn bfs(graph: &Graph, start: usize) -> HashMap<usize, usize> {
    let mut distances = HashMap::with_capacity(graph.node_count());
    let mut queue = VecDeque::with_capacity(graph.node_count());
    distances.insert(start, 0);
    queue.push_back(start);
    while let Some(node) = queue.pop_front() {
        if let Some(neighbours) = graph.adjacency_list().get(&node) {
            for &neighbour in neighbours {
                if !distances.contains_key(&neighbour) {
                    distances.insert(neighbour, distances[&node] + 1);
                    queue.push_back(neighbour);
                }
            }
        }
    }
    distances
}
