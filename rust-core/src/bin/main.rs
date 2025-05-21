use rust_core::Graph;

fn main() -> rust_core::Result<()> {
    let graph = Graph::from_msgpack("../.storage/processed_CA-GrQc.msgpack")?;
    println!("{:?}", graph);
    Ok(())
}
