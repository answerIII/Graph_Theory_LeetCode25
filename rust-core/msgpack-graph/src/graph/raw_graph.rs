use serde::Deserialize;
use std::{fs, path::Path};

#[derive(Deserialize, Debug)]
pub struct RawGraph {
    pub(crate) node_count: usize,
    pub(crate) edge_count: usize,
    pub(crate) edges: Vec<[usize; 2]>,
}

impl RawGraph {
    pub fn from_msgpack(path: impl AsRef<Path>) -> crate::Result<Self> {
        let bytes = fs::read(path)?;
        let raw_graph: RawGraph = rmp_serde::from_slice(&bytes)?;
        Ok(raw_graph)
    }
}
