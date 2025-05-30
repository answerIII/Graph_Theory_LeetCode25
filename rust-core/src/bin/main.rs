use msgpack_graph::{Graph, RMPSupport, RawGraph, Selection};
use serde::{Deserialize, Serialize};
use std::time::Instant;

#[derive(argh::FromArgs)]
/// NATS - ClickHouse. Microservice is designed to save data coming from NATS JetStream to ClickHouse.
struct Args {
    /// graph file name
    #[argh(option)]
    file_name: String,
    /// JSON request
    #[argh(option)]
    json: String,
}

#[derive(Deserialize, Debug)]
struct Request {
    start_node: usize,
    end_node: usize,
    algorithm: String,
    landmarks: Option<Landmarks>,
}

#[derive(Deserialize, Debug)]
struct Landmarks {
    count: usize,
    selection: String,
}

#[derive(Serialize, Debug)]
struct Response {
    algorithm: String,
    distance: Option<usize>,
    execution_time: f64,
    landmarks_count: usize,
    start_node: usize,
    end_node: usize,
}

fn main() -> rust_core::Result<()> {
    let args: Args = argh::from_env();
    let cache_path = format!(".cache/{}.msgpack", args.file_name);
    let storage_path = format!("../.storage/{}.msgpack", args.file_name);
    let mut graph = match Graph::from_msgpack(&cache_path) {
        Ok(graph) => {
            tracing::info!("Uploaded `{}` from: {}", args.file_name, cache_path);
            graph
        }
        Err(_) => {
            let raw_graph = RawGraph::from_msgpack(&storage_path)?;
            tracing::info!("Uploaded `{}` from: {}", args.file_name, storage_path);
            let graph = Graph::from(raw_graph);
            graph.to_msgpack(cache_path)?;
            graph
        }
    };
    let request: Request = serde_json::from_str(&args.json)?;
    match request.algorithm.as_str() {
        "bfs" => {
            let timestamp = Instant::now();
            let distance = graph.distance(request.start_node, request.end_node);
            println!(
                "{}",
                serde_json::to_string::<Response>(&Response {
                    algorithm: request.algorithm,
                    distance: distance,
                    execution_time: timestamp.elapsed().as_micros() as f64 / 1000000.0,
                    landmarks_count: graph.landmarks_count(),
                    start_node: request.start_node,
                    end_node: request.end_node,
                })?
            );
        }
        "landmarks-basic" => {
            let landmarks = request.landmarks.unwrap();
            match landmarks.selection.as_str() {
                "random" => graph.create_landmarks(landmarks.count, Selection::Random),
                "highest_degree" => graph.create_landmarks(landmarks.count, Selection::HighDegree),
                "max_coverage" => graph.create_landmarks(landmarks.count, Selection::BestCoverage),
                _ => {}
            }
            let timestamp = Instant::now();
            let estimate_distance = graph.estimate_distance(request.start_node, request.end_node);
            println!(
                "{}",
                serde_json::to_string::<Response>(&Response {
                    algorithm: request.algorithm,
                    distance: estimate_distance,
                    execution_time: timestamp.elapsed().as_micros() as f64 / 1000000.0,
                    landmarks_count: graph.landmarks_count(),
                    start_node: request.start_node,
                    end_node: request.end_node,
                })?
            );
        }
        "landmarks-bfs" => {
            let landmarks = request.landmarks.unwrap();
            match landmarks.selection.as_str() {
                "random" => graph.create_landmarks(landmarks.count, Selection::Random),
                "highest_degree" => graph.create_landmarks(landmarks.count, Selection::HighDegree),
                "max_coverage" => graph.create_landmarks(landmarks.count, Selection::BestCoverage),
                _ => {}
            }
            let timestamp = Instant::now();
            let estimate_distance_bfs =
                graph.estimate_distance_bfs(request.start_node, request.end_node);
            println!(
                "{}",
                serde_json::to_string::<Response>(&Response {
                    algorithm: request.algorithm,
                    distance: estimate_distance_bfs,
                    execution_time: timestamp.elapsed().as_micros() as f64 / 1000000.0,
                    landmarks_count: graph.landmarks_count(),
                    start_node: request.start_node,
                    end_node: request.end_node,
                })?
            );
        }
        _ => {}
    }
    Ok(())
}
