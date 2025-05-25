use crate::{Graph, RMPSupport};
use rand::{rng, seq::SliceRandom};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

pub enum Selection {
    Random,
    Degree,
    Coverage,
}

#[derive(Serialize, Deserialize)]
pub struct Landmarks {
    distances: HashMap<usize, HashMap<usize, usize>>,
}

impl RMPSupport for Landmarks {}

impl Landmarks {
    pub fn new(graph: &Graph, number_of_landmarks: usize, selection: Selection) -> Self {
        let landmarks = match selection {
            Selection::Random => Self::select_random_landmarks(&graph, number_of_landmarks),
            Selection::Degree => Self::select_high_degree_landmarks(&graph, number_of_landmarks),
            Selection::Coverage => Self::select_coverage_landmarks(&graph, number_of_landmarks),
        };
        let mut distances = HashMap::new();
        for landmark in landmarks {
            distances.insert(landmark, graph.distances(landmark));
        }
        Self { distances }
    }

    /// Landmarks-Basic: distance estimation through landmarks from **`start`** to **`end`**
    pub fn estimate_distance(&self, start: usize, end: usize) -> Option<usize> {
        let mut distance: Option<usize> = None;
        for (_, distances) in &self.distances {
            if let (Some(&distance_to_start), Some(&distance_to_end)) =
                (distances.get(&start), distances.get(&end))
            {
                distance = Some(match distance {
                    Some(distance) => distance.min(distance_to_start + distance_to_end),
                    None => distance_to_start + distance_to_end,
                });
            }
        }
        distance
    }

    /// Landmarks-BFS: distance estimation through landmarks from **`start`** to **`end`**
    pub fn estimate_distance_bfs(&self, start: usize, end: usize) -> Option<usize> {
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
        let degrees = graph.degrees();
        graph.degrees().sort_by(|a, b| b.1.cmp(&a.1));
        degrees
            .into_iter()
            .take(number_of_landmarks)
            .map(|(node, _)| node)
            .collect()
    }

    fn select_coverage_landmarks(graph: &Graph, number_of_landmarks: usize) -> Vec<usize> {
        todo!()
    }
}
