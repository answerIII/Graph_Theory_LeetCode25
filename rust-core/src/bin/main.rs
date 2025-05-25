use msgpack_graph::{Graph, RMPSupport, RawGraph};
use std::time::Instant;

fn main() -> msgpack_graph::Result<()> {
    tracing_subscriber::fmt()
        .with_max_level(tracing::Level::DEBUG)
        .init();
    let _ = std::fs::create_dir(".cache");
    let filename = "vk.msgpack";
    let timestamp = Instant::now();
    let graph = match Graph::from_msgpack(format!(".cache/{}", filename)) {
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
        graph.edge_count()
    );
    tracing::info!("Graph loading time: {} ms", timestamp.elapsed().as_millis());
    Ok(())
}
