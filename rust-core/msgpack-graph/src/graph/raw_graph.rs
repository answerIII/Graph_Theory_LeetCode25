use serde::Deserialize;
use std::{fs, path::Path};

#[derive(Deserialize, Debug)]
pub struct RawGraph {
    node_count: usize,
    edge_count: usize,
    edges: Vec<[usize; 2]>,
}

impl RawGraph {
    pub fn from_msgpack(path: impl AsRef<Path>) -> crate::Result<Self> {
        let bytes = fs::read(path)?;
        let raw_graph: RawGraph = rmp_serde::from_slice(&bytes)?;
        Ok(raw_graph)
    }

    pub fn node_count(&self) -> usize {
        self.node_count
    }

    pub fn edge_count(&self) -> usize {
        self.edge_count
    }

    pub fn edges(&self) -> &Vec<[usize; 2]> {
        &self.edges
    }
}
