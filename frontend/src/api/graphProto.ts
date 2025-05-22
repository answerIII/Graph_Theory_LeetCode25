
// import { loadProto } from '../proto/graph_pb';
// import { GraphServiceClient } from '../proto/graph_grpc_web_pb';
// import type { Graph as GraphType, Edge as EdgeType } from '../types/graph';

// // Инициализация gRPC-клиента
// const client = new GraphServiceClient('http://localhost:8080', null, null);

// export const uploadGraph = async (graph: GraphType): Promise<any> => {
//     const { Graph, Edge, UploadResponse } = await loadProto();

//     // Создаём Protobuf-сообщение
//     const request = Graph.create();
//     request.directed = graph.directed;
//     request.numNodes = graph.numNodes;
//     request.numEdges = graph.numEdges;
//     request.edges = graph.edges.map(e => {
//         const edge = Edge.create();
//         edge.from = e.from;
//         edge.to = e.to;
//         return edge;
//     });

//     return new Promise((resolve, reject) => {
//         client.uploadGraph(request, {}, (err: any, response: any) => {
//             if (err) {
//                 reject(new Error(`gRPC error: ${err.message}`));
//                 return;
//             }
//             resolve(UploadResponse.toObject(false, response));
//         });
//     });
// };

// export const streamGraph = async (graph: GraphType, chunkSize: number = 100000): Promise<any> => {
//     const { GraphChunk, Edge, UploadResponse } = await loadProto();

//     return new Promise((resolve, reject) => {
//         const stream = client.streamGraph({}, (err: any, response: any) => {
//             if (err) {
//                 reject(new Error(`gRPC stream error: ${err.message}`));
//                 return;
//             }
//             resolve(UploadResponse.toObject(false, response));
//         });

//         for (let i = 0; i < graph.edges.length; i += chunkSize) {
//             const chunk = GraphChunk.create();
//             const edges = graph.edges.slice(i, i + chunkSize);
//             chunk.edges = edges.map(e => {
//                 const edge = Edge.create();
//                 edge.from = e.from;
//                 edge.to = e.to;
//                 return edge;
//             });
//             stream.write(chunk);
//         }
//         stream.end();
//     });
// };