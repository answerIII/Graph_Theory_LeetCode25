use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
pub struct BFSNodeState {
    distance: usize,
    previous_node: Option<usize>,
}

impl BFSNodeState {
    pub fn from(distance: usize, previous_node: Option<usize>) -> Self {
        Self {
            distance,
            previous_node,
        }
    }

    pub fn distance(&self) -> usize {
        self.distance
    }

    pub fn previous_node(&self) -> Option<usize> {
        self.previous_node
    }
}