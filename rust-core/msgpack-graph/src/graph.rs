use crate::{RMPSupport, RawGraph};
use serde::{Deserialize, Serialize};
use std::collections::{HashMap, VecDeque};

#[derive(Serialize, Deserialize)]
pub struct Graph {
    node_count: usize,
    edge_count: usize,
    adjacency_list: HashMap<usize, Vec<usize>>,
}

impl RMPSupport for Graph {}

impl From<RawGraph> for Graph {
    fn from(raw_graph: RawGraph) -> Self {
        let mut adjacency_list: HashMap<usize, Vec<usize>> =
            HashMap::with_capacity(raw_graph.node_count);
        for edge in raw_graph.edges {
            adjacency_list.entry(edge[0]).or_default().push(edge[1]);
        }
        Self {
            node_count: raw_graph.node_count,
            edge_count: raw_graph.edge_count,
            adjacency_list,
        }
    }
}

impl Graph {
    /// Breadth-First Search: finds the shortest paths from **`start`** to **all other reachable**
    pub fn distances(&self, start: usize) -> HashMap<usize, usize> {
        let mut distances = HashMap::with_capacity(self.node_count);
        let mut queue = VecDeque::with_capacity(self.node_count);
        distances.insert(start, 0);
        queue.push_back(start);
        while let Some(node) = queue.pop_front() {
            if let Some(neighbours) = self.adjacency_list.get(&node) {
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

    pub fn node_count(&self) -> usize {
        self.node_count
    }

    pub fn edge_count(&self) -> usize {
        self.edge_count
    }

    pub fn adjacency_list(&self) -> &HashMap<usize, Vec<usize>> {
        &self.adjacency_list
    }

    pub fn degrees(&self) -> Vec<(usize, usize)> {
        self.adjacency_list
            .iter()
            .map(|(&node, neighbors)| (node, neighbors.len()))
            .collect()
    }
}
