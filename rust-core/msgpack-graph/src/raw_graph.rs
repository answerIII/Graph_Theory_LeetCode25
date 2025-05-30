use crate::RMPSupport;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
pub struct RawGraph {
    #[serde(rename = "nodeCount")]
    node_count: usize,
    edges: Vec<[usize; 2]>,
}

impl RMPSupport for RawGraph {}

impl RawGraph {
    pub fn node_count(&self) -> usize {
        self.node_count
    }

    pub fn edges(&self) -> &Vec<[usize; 2]> {
        &self.edges
    }
}
