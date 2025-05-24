package structs

type AnswerA1 struct {
	WCC           *Graph  `json:"-" msgpack:"-"`
	Directed      bool    `json:"directed" msgpack:"directed"`
	VertexCount   int     `json:"vertexCount" msgpack:"vertexCount"`
	EdgesCount    int     `json:"edgesCount" msgpack:"edgesCount"`
	Density       float64 `json:"density" msgpack:"density"`
	ProportionWCC float32 `json:"propMaxWeakComponent" msgpack:"propMaxWeakComponent"`
	WCCCount      int     `json:"weakComponentCount" msgpack:"weakComponentCount"`
	ProportionSCC float32 `json:"propMaxStrongComponent" msgpack:"propMaxStrongComponent"`
	SCCCount      int     `json:"strongComponentCount" msgpack:"strongComponentCount"`
}

type AnswerA2 struct {
	Diameter     int
	Percentile90 float32
}
