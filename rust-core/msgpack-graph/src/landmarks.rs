use crate::{Graph, RMPSupport, graph::BFSNodeState};
use rand::{rng, seq::SliceRandom};
use rayon::iter::{IntoParallelIterator, ParallelIterator};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Serialize, Deserialize)]
struct Landmark {
    landmark: usize,
    distances: HashMap<usize, BFSNodeState>,
}

impl Landmark {
    fn path_to(&self, mut node: usize) -> Vec<usize> {
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
pub struct Landmarks {
    landmarks: Vec<Landmark>,
}

impl RMPSupport for Landmarks {}

pub enum Selection {
    Random,
    Degree,
    Coverage,
}

impl Landmarks {
    pub fn new(graph: &Graph, number_of_landmarks: usize, selection: Selection) -> Self {
        let landmarks = match selection {
            Selection::Random => Self::select_random_landmarks(&graph, number_of_landmarks),
            Selection::Degree => Self::select_high_degree_landmarks(&graph, number_of_landmarks),
            Selection::Coverage => {
                Self::select_best_coverage_landmarks(&graph, number_of_landmarks)
            }
        };
        Self {
            landmarks: landmarks
                .into_par_iter()
                .map(|landmark| Landmark {
                    landmark,
                    distances: graph.shortest_paths(landmark),
                })
                .collect(),
        }
    }

    /// Landmarks-Basic: distance estimation through landmarks
    pub fn estimate_distance(&self, start: usize, end: usize) -> Option<usize> {
        let mut distance: Option<usize> = None;
        for landmark in &self.landmarks {
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
    pub fn estimate_distance_bfs(&self, start: usize, end: usize) {
        todo!()
    }
}

impl Landmarks {
    fn select_random_landmarks(graph: &Graph, number_of_landmarks: usize) -> Vec<usize> {
        let mut nodes: Vec<usize> = graph.adjacency_list().keys().copied().collect();
        nodes.shuffle(&mut rng());
        nodes.into_iter().take(number_of_landmarks).collect()
    }

    fn select_high_degree_landmarks(graph: &Graph, number_of_landmarks: usize) -> Vec<usize> {
        let mut degrees = graph.degrees();
        degrees.sort_by(|a, b| b.1.cmp(&a.1));
        degrees
            .into_iter()
            .take(number_of_landmarks)
            .map(|(node, _)| node)
            .collect()
    }

    fn select_best_coverage_landmarks(graph: &Graph, number_of_landmarks: usize) -> Vec<usize> {
        todo!()
    }
}
