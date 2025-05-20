export interface Edge{
    from: number;
    to: number;
}

export interface Graph{
    directed: boolean;
    numNodes: number;
    numEdges: number;
    edges: Edge[];
}