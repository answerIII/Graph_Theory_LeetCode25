// Типы для компонента GeneralProperties

export interface Graph {
  directed: boolean;
  vertexCount: number;
  edgesCount: number;
  adjList: Map<string, string[]>;
  edges: Array<{ from: string; to: string }>;
}

export interface StatDFS {
  vertex: string;
  timeIn: number;
  timeOut: number;
}

export interface LogEntry {
  message: string;
  timestamp: number;
}

export interface GeneralPropertiesData {
  directed: boolean;
  nodeCount: number;
  edgesCount: number;
  density: number;
  wccCount: number;
  proportionWCC: number;
  sccCount?: number;
  proportionSCC?: number;
}