use crate::{
    RMPSupport, RawGraph,
    utils::{BFSNodeState, ShortestPathTree},
};
use rand::{rng, seq::SliceRandom};
use rayon::iter::{IntoParallelIterator, ParallelIterator};
use serde::{Deserialize, Serialize};
use std::collections::{HashMap, HashSet, VecDeque};

#[derive(Serialize, Deserialize)]
pub struct Graph {
    node_count: usize,
    edge_count: usize,
    landmarks: Option<Vec<ShortestPathTree>>,
    adjacency_list: HashMap<usize, Vec<usize>>,
}

pub enum Selection {
    Random,
    Degree,
    BestCoverage,
}

impl RMPSupport for Graph {}

impl From<RawGraph> for Graph {
    fn from(raw_graph: RawGraph) -> Self {
        let mut adjacency_list: HashMap<usize, Vec<usize>> =
            HashMap::with_capacity(raw_graph.node_count());
        for edge in raw_graph.edges() {
            adjacency_list.entry(edge[0]).or_default().push(edge[1]);
        }
        Self {
            node_count: raw_graph.node_count(),
            edge_count: raw_graph.edge_count(),
            landmarks: None,
            adjacency_list,
        }
    }
}

impl Graph {
    pub fn create_landmarks(&mut self, n: usize, selection: Selection) {
        self.landmarks = Some(
            match selection {
                Selection::Random => self.select_random_nodes(n),
                Selection::Degree => self.select_high_degree_nodes(n),
                Selection::BestCoverage => self.select_best_coverage_nodes(n),
            }
            .into_par_iter()
            .map(|node| self.shortest_path_tree(node))
            .collect(),
        );
    }

    pub fn select_random_nodes(&self, n: usize) -> Vec<usize> {
        let mut nodes: Vec<usize> = self.adjacency_list().keys().copied().collect();
        nodes.shuffle(&mut rng());
        nodes.into_iter().take(n).collect()
    }

    pub fn select_high_degree_nodes(&self, n: usize) -> Vec<usize> {
        let mut degrees = self.degrees();
        degrees.sort_by(|a, b| b.1.cmp(&a.1));
        degrees.into_iter().take(n).map(|(node, _)| node).collect()
    }

    pub fn select_best_coverage_nodes(&self, n: usize) -> Vec<usize> {
        let paths: HashSet<Vec<usize>> = (0..100)
            .into_par_iter()
            .filter_map(|_| {
                let nodes = self.select_random_nodes(2);
                self.shortest_path(nodes[0], nodes[1])
            })
            .collect();
        let mut nodes = HashSet::with_capacity(n);
        for _ in 0..n {
            let mut covered = HashMap::new();
            for path in &paths {
                if path.iter().all(|path_node| !nodes.contains(path_node)) {
                    path.iter()
                        .for_each(|&node| *covered.entry(node).or_insert(0) += 1);
                }
            }
            if let Some((node, _)) = covered.into_iter().max() {
                nodes.insert(node);
            }
        }
        nodes.into_iter().collect()
    }

    pub fn node_count(&self) -> usize {
        self.node_count
    }

    pub fn edge_count(&self) -> usize {
        self.edge_count
    }

    pub fn landmarks_count(&self) -> usize {
        self.landmarks
            .as_ref()
            .map_or(0, |landmarks| landmarks.len())
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

impl Graph {
    /// Breadth-First Search: finds the shortest distance
    pub fn distance(&self, start: usize, end: usize) -> Option<usize> {
        let mut distances = HashMap::with_capacity(self.node_count);
        let mut queue = VecDeque::with_capacity(self.node_count);
        distances.insert(start, 0);
        queue.push_back(start);
        while let Some(node) = queue.pop_front() {
            if let Some(neighbours) = self.adjacency_list.get(&node) {
                for &neighbour in neighbours {
                    if !distances.contains_key(&neighbour) {
                        distances.insert(neighbour, distances[&node] + 1);
                        if neighbour == end {
                            return distances.get(&neighbour).copied();
                        }
                        queue.push_back(neighbour);
                    }
                }
            }
        }
        None
    }

    /// Landmarks-Basic: distance estimation through landmarks
    pub fn estimate_distance(&self, start: usize, end: usize) -> Option<usize> {
        let Some(landmarks) = self.landmarks.as_ref() else {
            panic!("Missing landmarks");
        };
        let mut distance: Option<usize> = None;
        landmarks.iter().for_each(|shortest_path_tree| {
            if let (Some(start), Some(end)) =
                (shortest_path_tree.get(&start), shortest_path_tree.get(&end))
            {
                distance = Some(match distance {
                    Some(distance) => distance.min(start.distance() + end.distance()),
                    None => start.distance() + end.distance(),
                });
            }
        });
        distance
    }

    /// Landmarks-BFS: distance estimation through landmarks
    pub fn estimate_distance_bfs(&self, start: usize, end: usize) -> Option<usize> {
        let Some(landmarks) = self.landmarks.as_ref() else {
            panic!("Missing landmarks");
        };
        let mut subgraph = HashSet::new();
        landmarks.iter().for_each(|landmark| {
            subgraph.extend(landmark.path_to(start));
            subgraph.extend(landmark.path_to(end));
        });
        self.distance_in_subgraph(subgraph, start, end)
    }
}

impl Graph {
    fn distance_in_subgraph(
        &self,
        subgraph: HashSet<usize>,
        start: usize,
        end: usize,
    ) -> Option<usize> {
        if !subgraph.contains(&start) || !subgraph.contains(&start) {
            return None;
        }
        let mut distances = HashMap::with_capacity(self.node_count);
        let mut queue = VecDeque::with_capacity(self.node_count);
        distances.insert(start, 0);
        queue.push_back(start);
        while let Some(node) = queue.pop_front() {
            if let Some(neighbours) = self.adjacency_list.get(&node) {
                for &neighbour in neighbours {
                    if !distances.contains_key(&neighbour) && subgraph.contains(&neighbour) {
                        distances.insert(neighbour, distances[&node] + 1);
                        if neighbour == end {
                            return distances.get(&neighbour).copied();
                        }
                        queue.push_back(neighbour);
                    }
                }
            }
        }
        None
    }

    fn shortest_path(&self, start: usize, end: usize) -> Option<Vec<usize>> {
        let mut shortest_path_tree = ShortestPathTree::new();
        let mut queue = VecDeque::with_capacity(self.node_count);
        shortest_path_tree.insert(start, BFSNodeState::from(0, None));
        queue.push_back(start);
        while let Some(node) = queue.pop_front() {
            if let Some(neighbours) = self.adjacency_list.get(&node) {
                for &neighbour in neighbours {
                    if !shortest_path_tree.contains_key(&neighbour) {
                        let distance = shortest_path_tree[&node].distance() + 1;
                        shortest_path_tree
                            .insert(neighbour, BFSNodeState::from(distance, Some(node)));
                        if neighbour == end {
                            return Some(shortest_path_tree.path_to(end));
                        }
                        queue.push_back(neighbour);
                    }
                }
            }
        }
        None
    }

    fn shortest_path_tree(&self, start: usize) -> ShortestPathTree {
        let mut shortest_path_tree = ShortestPathTree::new();
        let mut queue = VecDeque::with_capacity(self.node_count);
        shortest_path_tree.insert(start, BFSNodeState::from(0, None));
        queue.push_back(start);
        while let Some(node) = queue.pop_front() {
            if let Some(neighbours) = self.adjacency_list.get(&node) {
                for &neighbour in neighbours {
                    if !shortest_path_tree.contains_key(&neighbour) {
                        let distance = shortest_path_tree[&node].distance() + 1;
                        shortest_path_tree
                            .insert(neighbour, BFSNodeState::from(distance, Some(node)));
                        queue.push_back(neighbour);
                    }
                }
            }
        }
        shortest_path_tree
    }
}
