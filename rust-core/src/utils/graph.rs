use serde::Deserialize;
use std::{collections::HashMap, fs, path::Path};

#[derive(Deserialize, Debug)]
pub struct Graph {
    directed: bool,
    #[serde(rename = "vertexCount")]
    vertex_count: usize,
    edges: Vec<[usize; 2]>,
    #[serde(skip)]
    adjacency_list: HashMap<usize, Vec<usize>>,
}

impl Graph {
    pub fn from_msgpack(path: impl AsRef<Path>) -> crate::Result<Self> {
        let buf = fs::read(path)?;
        let mut graph: Self = rmp_serde::from_slice(&buf)?;
        for edge in &graph.edges {
            graph
                .adjacency_list
                .entry(edge[0])
                .or_default()
                .push(edge[1]);
        }
        Ok(graph)
    }

    pub fn is_directed(&self) -> bool {
        self.directed
    }
}
