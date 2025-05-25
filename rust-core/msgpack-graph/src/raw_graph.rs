use crate::RMPSupport;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug)]
pub struct RawGraph {
    #[serde(rename = "nodeCount")]
    pub(crate) node_count: usize,
    #[serde(rename = "edgeCount")]
    pub(crate) edge_count: usize,
    pub(crate) edges: Vec<[usize; 2]>,
}

impl RMPSupport for RawGraph {}
