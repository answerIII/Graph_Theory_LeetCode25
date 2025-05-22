const protobuf = require('protobufjs');

// Загружаем .proto файл
const loadProto = async () => {
    const root = await protobuf.load('src/proto/graph.proto');
    return {
        Edge: root.lookupType('graph.Edge'),
        Graph: root.lookupType('graph.Graph'),
        GraphChunk: root.lookupType('graph.GraphChunk'),
        UploadResponse: root.lookupType('graph.UploadResponse'),
    };
};

module.exports = { loadProto };