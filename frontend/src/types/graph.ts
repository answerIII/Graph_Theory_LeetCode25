export interface Graph {
    directed: boolean;
    numNodes: number;
    edges: [number, number][];
}