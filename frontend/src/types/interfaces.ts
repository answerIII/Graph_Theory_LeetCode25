export interface Graph {
  directed: boolean;
  numNodes: number;
  edges: [number, number][];
}

export interface GraphUpload {
  directed: boolean;
  numNodes: number;
  edgeCount: number;
  maxVertexId: number;
  edges: [number, number][];
}

export interface MessagePackData {
  is_directed: boolean;
  nodeCount: number;
  edgeCount?: number;
  edges?: [number, number][];
}

export interface WorkerMessage {
    file: File;
    directed: string;
    isVeryLargeGraph?: boolean;
}

export interface WorkerResponse {
    type: 'result' | 'progress' | 'error' | 'log';
    graph?: GraphUpload;
    progress?: number;
    error?: string;
    log?: string;
}