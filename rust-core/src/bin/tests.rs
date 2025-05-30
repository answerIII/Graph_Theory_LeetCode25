use msgpack_graph::{Graph, RMPSupport, RawGraph, Selection};
use std::time::Instant;

fn main() -> msgpack_graph::Result<()> {
    tracing_subscriber::fmt()
        .with_max_level(tracing::Level::DEBUG)
        .init();
    let storage = std::fs::read_dir("../.storage")?;
    for entry in storage {
        if let Ok(entry) = entry {
            if let Some(file_name) = entry.file_name().to_str() {
                let storage_path = format!("../.storage/{}", file_name);
                tracing::info!("Uploaded `{}` from: {}", file_name, storage_path);
                let raw_graph = RawGraph::from_msgpack(&storage_path)?;
                let mut graph = Graph::from(raw_graph);
                if let Some((&max, _)) = graph.adjacency_list().iter().max() {
                    let timestamp = Instant::now();
                    let distance = graph.distance(max, 7).unwrap();
                    tracing::info!(
                        "BFS distance({}, {}): {:?}, time: {} secs",
                        max,
                        7,
                        distance,
                        timestamp.elapsed().as_micros() as f64 / 1000000.0
                    );
                    for n in [20, 100] {
                        estimate_distance(&mut graph, Selection::Random, n, max, 7);
                        estimate_distance(&mut graph, Selection::HighDegree, n, max, 7);
                        estimate_distance(&mut graph, Selection::BestCoverage, n, max, 7);
                    }
                    tracing::debug!("");
                }
            }
        }
    }
    Ok(())
}

fn estimate_distance(graph: &mut Graph, selection: Selection, n: usize, start: usize, end: usize) {
    let mut timestamp = Instant::now();
    let landmarks_selection = selection.to_string();
    graph.create_landmarks(n, selection);
    let create_landmarks_time = timestamp.elapsed().as_micros() as f64 / 1000000.0;
    tracing::info!(
        "Creating `{}` `{}` landmarks time: {} secs",
        landmarks_selection,
        graph.landmarks_count(),
        create_landmarks_time
    );
    timestamp = Instant::now();
    let estimate_distance = graph.estimate_distance(start, end).unwrap();
    let estimate_distance_time = timestamp.elapsed().as_micros() as f64 / 1000000.0;
    tracing::info!(
        "Landmark-Basic estimate distance({}, {}): {:?}, time: {} secs",
        start,
        end,
        estimate_distance,
        estimate_distance_time
    );
    timestamp = Instant::now();
    let estimate_distance_bfs = graph.estimate_distance_bfs(start, end).unwrap();
    let estimate_distance_bfs_time = timestamp.elapsed().as_micros() as f64 / 1000000.0;
    tracing::info!(
        "Landmark-BFS estimate distance({}, {}): {:?}, time: {} secs",
        start,
        end,
        estimate_distance_bfs,
        estimate_distance_bfs_time
    );
}
