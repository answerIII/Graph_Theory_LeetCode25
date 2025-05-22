const { GrpcWebClientBase, MethodType } = require('grpc-web');

class GraphServiceClient {
    constructor(address, credentials, options) {
        this.client_ = new GrpcWebClientBase({ format: 'text' });
        this.address_ = address;
        this.credentials_ = credentials || null;
        this.options_ = options || {};
    }

    uploadGraph(request, metadata, callback) {
        const methodDescriptor = {
            path: '/graph.GraphService/UploadGraph',
            requestType: 'graph.Graph',
            responseType: 'graph.UploadResponse',
            requestStream: false,
            responseStream: false,
        };
        return this.client_.unaryCall(
            this.address_ + methodDescriptor.path,
            request,
            metadata || {},
            methodDescriptor,
            callback
        );
    }

    streamGraph(metadata, callback) {
        const methodDescriptor = {
            path: '/graph.GraphService/StreamGraph',
            requestType: 'graph.GraphChunk',
            responseType: 'graph.UploadResponse',
            requestStream: true,
            responseStream: false,
        };
        const stream = this.client_.clientStreamingCall(
            this.address_ + methodDescriptor.path,
            metadata || {},
            methodDescriptor,
            callback
        );
        return stream;
    }
}

module.exports = { GraphServiceClient };