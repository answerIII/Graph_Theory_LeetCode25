use msgpack_graph::{Graph, RMPSupport, RawGraph, Selection};
use std::time::Instant;

fn main() -> msgpack_graph::Result<()> {
    tracing_subscriber::fmt()
        .with_max_level(tracing::Level::DEBUG)
        .init();
    let _ = std::fs::create_dir(".cache");
    let filename = "processed_vk.msgpack";
    let mut timestamp = Instant::now();
    let mut graph = match Graph::from_msgpack(format!(".cache/{}", filename)) {
        Ok(graph) => {
            tracing::info!("Uploaded from: .cache/{}", filename);
            graph
        }
        Err(_) => {
            let raw_graph = RawGraph::from_msgpack(format!("../.storage/{}", filename))?;
            tracing::info!("Uploaded from: .storage/{}", filename);
            let graph = Graph::from(raw_graph);
            graph.to_msgpack(format!(".cache/{}", filename))?;
            graph
        }
    };
    tracing::info!(
        "Nodes: {}, Edges: {}",
        graph.node_count(),
        graph.edge_count(),
    );
    tracing::info!("Loading time: {} ms", timestamp.elapsed().as_millis());
    timestamp = Instant::now();
    graph.create_landmarks(100, Selection::Degree);
    tracing::info!(
        "Creating {} landmarks time: {} ms",
        graph.landmarks_count(),
        timestamp.elapsed().as_millis()
    );
    // graph.to_msgpack(format!(".cache/{}", filename))?;
    timestamp = Instant::now();
    let distance = graph.distance(5252845, 15196597);
    tracing::info!(
        "BFS distance: {:?}, time: {} ms",
        distance,
        timestamp.elapsed().as_millis()
    );
    timestamp = Instant::now();
    let estimate_distance = graph.estimate_distance(5252845, 15196597);
    tracing::info!(
        "Landmark-Basic estimate distance: {:?}, time: {} ms",
        estimate_distance,
        timestamp.elapsed().as_millis()
    );
    timestamp = Instant::now();
    let estimate_distance = graph.estimate_distance_bfs(5252845, 15196597);
    tracing::info!(
        "Landmark-BFS estimate distance: {:?}, time: {} ms",
        estimate_distance,
        timestamp.elapsed().as_millis()
    );
    Ok(())
}
