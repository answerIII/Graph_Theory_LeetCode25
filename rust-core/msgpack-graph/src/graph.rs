use crate::{RMPSupport, RawGraph};
use rand::{rng, seq::SliceRandom};
use rayon::iter::{IntoParallelIterator, ParallelIterator};
use serde::{Deserialize, Serialize};
use std::collections::{HashMap, HashSet, VecDeque};

#[derive(Serialize, Deserialize)]
pub struct Landmark {
    landmark: usize,
    distances: HashMap<usize, BFSNodeState>,
}

impl Landmark {
    pub fn path_to(&self, mut node: usize) -> Vec<usize> {
        let mut path = vec![node];
        while let Some(previous_node) = self
            .distances
            .get(&node)
            .and_then(|state| state.previous_node)
        {
            path.push(previous_node);
            node = previous_node;
        }
        path
    }
}

#[derive(Serialize, Deserialize)]
pub struct Graph {
    node_count: usize,
    edge_count: usize,
    landmarks: Option<Vec<Landmark>>,
    adjacency_list: HashMap<usize, Vec<usize>>,
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

pub enum Selection {
    Random,
    Degree,
    Coverage,
}

impl Graph {
    pub fn create_landmarks(&mut self, number_of_landmarks: usize, selection: Selection) {
        self.landmarks = Some(
            match selection {
                Selection::Random => self.select_random_landmarks(number_of_landmarks),
                Selection::Degree => self.select_high_degree_landmarks(number_of_landmarks),
                Selection::Coverage => self.select_best_coverage_landmarks(number_of_landmarks),
            }
            .into_par_iter()
            .map(|landmark| Landmark {
                landmark,
                distances: self.shortest_paths(landmark),
            })
            .collect(),
        );
    }

    /// Breadth-First Search: finds the shortest distance from **`start`** to **`end`**
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
        for landmark in landmarks.iter() {
            if let (Some(start), Some(end)) =
                (landmark.distances.get(&start), landmark.distances.get(&end))
            {
                distance = Some(match distance {
                    Some(distance) => distance.min(start.distance + end.distance),
                    None => start.distance + end.distance,
                });
            }
        }
        distance
    }

    /// Landmarks-BFS: distance estimation through landmarks from **`start`** to **`end`**
    pub fn estimate_distance_bfs(&self, start: usize, end: usize) -> Option<usize> {
        let Some(landmarks) = self.landmarks.as_ref() else {
            panic!("Missing landmarks");
        };
        let mut subgraph = HashSet::new();
        landmarks.iter().for_each(|landmark| {
            subgraph.extend(landmark.path_to(start));
            subgraph.extend(landmark.path_to(end));
        });
        self.distance_with_subgraph(subgraph, start, end)
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

#[derive(Serialize, Deserialize)]
struct BFSNodeState {
    distance: usize,
    previous_node: Option<usize>,
}

impl Graph {
    /// Breadth-First Search: finds the shortest paths from **`start`** to **all other reachable**
    fn shortest_paths(&self, start: usize) -> HashMap<usize, BFSNodeState> {
        let mut distances = HashMap::with_capacity(self.node_count);
        let mut queue = VecDeque::with_capacity(self.node_count);
        distances.insert(
            start,
            BFSNodeState {
                distance: 0,
                previous_node: None,
            },
        );
        queue.push_back(start);
        while let Some(node) = queue.pop_front() {
            if let Some(neighbours) = self.adjacency_list.get(&node) {
                for &neighbour in neighbours {
                    if !distances.contains_key(&neighbour) {
                        distances.insert(
                            neighbour,
                            BFSNodeState {
                                distance: distances[&node].distance + 1,
                                previous_node: Some(node),
                            },
                        );
                        queue.push_back(neighbour);
                    }
                }
            }
        }
        distances
    }

    /// Breadth-First Search: finds the shortest distance from **`start`** to **`end`** with subgraph
    fn distance_with_subgraph(
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

    fn select_random_landmarks(&self, number_of_landmarks: usize) -> Vec<usize> {
        let mut nodes: Vec<usize> = self.adjacency_list().keys().copied().collect();
        nodes.shuffle(&mut rng());
        nodes.into_iter().take(number_of_landmarks).collect()
    }

    fn select_high_degree_landmarks(&self, number_of_landmarks: usize) -> Vec<usize> {
        let mut degrees = self.degrees();
        degrees.sort_by(|a, b| b.1.cmp(&a.1));
        degrees
            .into_iter()
            .take(number_of_landmarks)
            .map(|(node, _)| node)
            .collect()
    }

    fn select_best_coverage_landmarks(&self, number_of_landmarks: usize) -> Vec<usize> {
        todo!()
    }
}
