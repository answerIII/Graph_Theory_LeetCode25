package structs

type Graph struct {
	Directed    bool          `default:"false"`
	VertexCount int           `deafult:"0"`
	EdgesCount  int           `deafult:"0"`
	AdjList     map[int][]int `default:"nil"`
	Edges       []Edge        `default:"nil"`
}

type GraphJSON struct {
	Directed bool   `json:"directed"`
	NumNodes int    `json:"numNodes"`
	NumEdges int    `json:"numEdges"`
	Edges    []Edge `json:"edges"`
}

type GraphMsg struct {
	Directed    bool    `msgpack:"directed"`
	VertexCount int     `msgpack:"nodeCount"`
	Edges       [][]int `msgpack:"edges"`
}

// type GraphMsg struct {
// 	Directed bool
// 	NumNodes int
// 	Edges    []Edge
// }

type Edge struct {
	From int `json:"from"`
	To   int `json:"to"`
}

type StatDFS struct {
	Vertex  int
	TimeIn  int
	TimeOut int
}
