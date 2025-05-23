use super::RawGraph;
use serde::{Deserialize, Serialize};
use std::{collections::{HashMap, HashSet}, fs, path::Path};

#[derive(Serialize, Deserialize)]
pub struct Graph {
    adjacency_list: HashMap<usize, HashSet<usize>>,
}

impl From<RawGraph> for Graph {
    fn from(raw_graph: RawGraph) -> Self {
        let mut adjacency_list: HashMap<usize, HashSet<usize>> =
            HashMap::with_capacity(raw_graph.node_count());
        for edge in raw_graph.edges() {
            adjacency_list.entry(edge[0]).or_default().insert(edge[1]);
        }
        Self { adjacency_list }
    }
}

impl Graph {
    pub fn from_msgpack(path: impl AsRef<Path>) -> crate::Result<Self> {
        let bytes = fs::read(path)?;
        let graph: Graph = rmp_serde::from_slice(&bytes)?;
        Ok(graph)
    }

    pub fn to_msgpack(&self, path: impl AsRef<Path>) -> crate::Result<()> {
        let bytes = rmp_serde::encode::to_vec(self)?;
        fs::write(path, bytes)?;
        Ok(())
    }
}