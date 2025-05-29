use super::BFSNodeState;
use serde::{Deserialize, Serialize};
use std::{
    collections::HashMap,
    ops::{Deref, DerefMut},
};

#[derive(Serialize, Deserialize)]
pub struct ShortestPathTree {
    shortest_path_tree: HashMap<usize, BFSNodeState>,
}

impl Deref for ShortestPathTree {
    type Target = HashMap<usize, BFSNodeState>;
    fn deref(&self) -> &Self::Target {
        &self.shortest_path_tree
    }
}

impl DerefMut for ShortestPathTree {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.shortest_path_tree
    }
}

impl ShortestPathTree {
    pub fn new() -> Self {
        Self {
            shortest_path_tree: HashMap::new(),
        }
    }

    pub fn path_to(&self, mut node: usize) -> Vec<usize> {
        let mut path = vec![node];
        while let Some(previous_node) = self
            .shortest_path_tree
            .get(&node)
            .and_then(|state| state.previous_node())
        {
            path.push(previous_node);
            node = previous_node;
        }
        path
    }
}
