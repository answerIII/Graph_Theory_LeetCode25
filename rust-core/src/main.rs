use msgpack_graph::graph::{Graph, RawGraph};
use std::time::Instant;

fn main() -> msgpack_graph::Result<()> {
    tracing_subscriber::fmt()
        .with_max_level(tracing::Level::DEBUG)
        .init();
    let _ = std::fs::create_dir(".cache");
    let filename = "processed_vk.msgpack";
    let timestamp = Instant::now();
    let graph = match Graph::from_msgpack(format!(".cache/{}", filename)) {
        Ok(graph) => {
            tracing::info!("Uploaded from: .cache/{}", filename);
            graph
        }
        Err(_) => {
            let raw_graph = RawGraph::from_msgpack(format!("../.storage/{}", filename))?;
            tracing::info!("Uploaded from: .storage/{}", filename);
            tracing::info!(
                "Nodes: {}, Edges: {}",
                raw_graph.node_count(),
                raw_graph.edge_count()
            );
            Graph::from(raw_graph)
        }
    };
    tracing::info!("Loading time: {} ms", timestamp.elapsed().as_millis());
    Ok(())
}
