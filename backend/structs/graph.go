package structs

type Graph struct {
	Directed    bool          `default:"false"`
	VertexCount int           `deafult:"0"`
	EdgesCount  int           `deafult:"0"`
	AdjList     map[int][]int `default:"nil"`
	Edges       []Edge        `default:"nil"`
	// WCC         map[int][]int `default:"nil"`
	// WCCCount    int           `default:"0"`
}

type GraphJSON struct {
	Directed bool   `json:"directed"`
	NumNodes int    `json:"numNodes"`
	NumEdges int    `json:"numEdges"`
	Edges    []Edge `json:"edges"`
}

type GraphMsg struct {
	Directed    bool    `msgpack:"is_directed"`
	VertexCount int     `msgpack:"nodeCount"`
	EdgesCount  int     `msgpack:"edgeCount"`
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

type ValueTime struct {
	Value  float64 `json:"value" msgpack:"value"`
	TimeMs int     `json:"execution_time_ms" msgpack:"execution_time_ms"`
}
